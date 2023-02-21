/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Cgi.cpp                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mtellal <marvin@42.fr>                     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/02/13 09:43:33 by mtellal           #+#    #+#             */
/*   Updated: 2023/02/13 09:43:35 by mtellal          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Cgi.hpp"
#include "utils.hpp"

Cgi::Cgi(const Cgi &src) :
_serv(src._serv), _req(src._req), _header(src._header), _raw_env(src._raw_env), _map_cgi(src._map_cgi) {}

Cgi::Cgi(const Server &serv, const Request &req, Header &header, char **env):
_serv(serv), _req(req), _header(header), _raw_env(env), _map_cgi(serv.getCgi())
{
    initEnv();
}

Cgi::~Cgi() {}

Cgi     &Cgi::operator=(const Cgi &src)
{
    if (this == &src)
    {
        this->_serv = src._serv;
        this->_req = src._req;
        this->_header = src._header;
        this->_header = src._header;
        this->_raw_env = src._raw_env;
        this->_env = src._env;
        this->_map_cgi = src._map_cgi;
    }
    return (*this);
}

Header          Cgi::getHeader() const { return (this->_header); }

size_t    tab_len(char **env)
{
    size_t  i = 0;
    
    while (env && env[i])
        i++;
    return (i);
}

int     Cgi::isCgiRequest()
{
    this->_map_cgi = this->_serv.getCgi();

    if (this->_map_cgi.size())
        return (1);
    else
        return (-1);

}


void    Cgi::printEnv()
{
    std::map<std::string, std::string>::iterator it = _env.begin();

    while (it != _env.end())
        std::cout << it->first << " = " << (it++)->second << std::endl;
}

void    Cgi::addEnvInMap()
{
    char    ** env;
    size_t  i;
    size_t  len;

    i = 0;
    env = this->_raw_env;
    len = tab_len(env);
    while (i < len)
    {
        std::string var_env;
        size_t      index;
        std::string key;
        std::string value;

        var_env = env[i];
        index = var_env.find("=");
        key = var_env.substr(0, index);
        value = var_env.substr(index + 1, var_env.length());
        _env[key] = value;
        i++;
    }
}

void    Cgi::initEnv()
{
    Request &req = this->_req;

    _env["PHPRC"]               =   "/etc/php/7.4/cgi/php.ini/";
    _env["REDIRECT_STATUS"]     =   "200";
    _env["AUTH_TYPE"]           =   req.getAuthentification();
    _env["CONTENT_LENGTH"]      =   req.getContentLength();
    _env["CONTENT_TYPE"]        =   req.getContentType();
    _env["GATEWAY_INTERFACE"]   =   "Cgi/1.1";
    _env["PATH_INFO"]           =   req.getPath();
    _env["PATH_TRANSLATED"]     =   req.getPath();
    _env["QUERY_STRING"]        =   "";
    _env["REMOTE_ADDR"]         =   ""; // need to be set
    _env["REMOTE_HOST"]         =   ""; // NULL
    _env["REMOTE_IDENT"]        =   ""; // NULL
    _env["REMOTE_USER"]         =   ""; // NULL
    _env["REMOTE_METHOD"]       =   req.getMethod();
    std::cout << "path = " << _serv.getRoot() << req.getPath() << std::endl;
    _env["SCRIPT_NAME"]         =  "." + _serv.getRoot() + req.getPath();
    _env["SCRIPT_FILENAME"]     =   "." + _serv.getRoot() + req.getPath();
    _env["SERVER_NAME"]         =   "";
    _env["SERVER_PORT"]         =   req.getPort();
    _env["SERVER_PROTOCOL"]     =   "HTTP/1.1";
    _env["SERVER_SOFTWARE"]     =   req.getServerName();

    addEnvInMap();

}

char    **Cgi::mapToTab()
{
    std::map<std::string, std::string>::iterator    it;
    char                                            **e;
    size_t                                          i;

    i = 0;
    it = this->_env.begin();
    try
    {
        e = new char*[this->_env.size() + 1];
    }
    catch (const std::exception &err)
    {
        std::cerr << "(mapToTab) creation of env general failed: " << err.what() << std::endl;
    }

    while (it != this->_env.end())
    {
        std::string  tmp;

        tmp = it->first + "=" + it->second;

        try 
        {
            e[i] = new char[tmp.length() + 1];
        }
        catch (const std::exception &err)
        {
            std::cerr << "(mapToTab) creation of var [" << i << "] failed: " << err.what() << std::endl;
        }

        strcpy(e[i], tmp.c_str());
        e[i][tmp.length()] = '\0';
        i++;
        it++;
    }
    e[i] = NULL;
    return (e);
}

char    **Cgi::exec_args(const std::string &path_file)
{
    char        **args;

    try
    {
        args = new char*[3];

        args[0] = strdup(this->_path_cgi.c_str());
        args[1] = strdup(path_file.c_str());
        args[2] = NULL;
    }
    catch (const std::exception &err)
    {
        std::cerr << " new call failed (exec_args)" << std::endl;
    }

    return (args);
}

void     Cgi::extractScript(std::string path_file)
{
    std::string                                     extension;
    std::map<std::string, std::string>::iterator    it;
    size_t                                          index;

    it =_map_cgi.begin();
    while (it != this->_map_cgi.end())
    {
        extension = ".";
        extension += it->first;
        if ((index = path_file.find(extension)) != (size_t)-1 && !path_file[index + extension.length()])
            this->_path_cgi = "." + it->second;            
        it++;
    }
}

void    Cgi::extractFields(const std::string &cgi_response)
{
    std::string                 header;
    std::vector<std::string>    lines;
    std::vector<std::string>    field;
    std::string                 tabFields[5] = 
        { "Status", "Content-Type", "PHP Warning" };


    header = cgi_response.substr(0, cgi_response.find("\n\r"));

    lines = ft_split(header, "\n");
    for (size_t i = 0; i < lines.size(); i++)
    {
        field = ft_split(lines[i], ":");

        if (field.size())
        {
            if (field[0] == "Content-type")
            {
                this->_header.setContentType(field[1]);
            }
        }
    }
}



int   Cgi::execute(const std::string &path_file, std::string &body)
{
    int             p[2];
    pid_t           f;

    char            **args;
    char            **env;

    std::string     header;
    size_t          index;

    env = mapToTab();
    args = exec_args(path_file);
    extractScript(path_file);

    std::cout << "this->_path_cgi = " << this->_path_cgi << std::endl;

    if (pipe(p) == -1)
    {
        perror("pipe call failed");
        return (500);
    }

    f = fork();
    if (f == -1)
    {
        perror("fork call failed");
        return (500);
    }
    if (f == 0)
    {
        // child
        close(p[0]);
        if (dup2(p[1], 1) == -1)
        {
            perror("dup2 call failed");
            exit(1);
        }

        if (execve(this->_path_cgi.c_str(), args, env) == -1)
        {
            perror("execve call failed");
            close(p[1]);
            exit(1);
        }
    }
    else
    {
        // pere
        close(p[1]);
        std::string respond;
        int len = 100;
        char buff[len];

        int bytes = read(p[0], buff, len - 1);
        buff[bytes] = '\0';
        respond.append(buff);

        while (bytes > 0)
        {
            bytes = read(p[0], buff, len - 1);
            buff[bytes] = '\0';
            respond.append(buff);
        }

        close(p[0]);

        index = respond.find("\n\r");

        if (index == (size_t)-1)
        {
            perror("no \\n\\r in cgi response, can't extract header-body: ");
            return (500);
        }

        header = respond.substr(0, respond.find("\n\r"));
        body = respond.substr(respond.find("\n\r") + 2, respond.length());

        std::cout << header << std::endl;

        extractFields(header);

        std::cout << body.length() << std::endl;

        this->_header.setContentLength(ft_itos(body.length()));

        return (200);
    }
    return (500);
}