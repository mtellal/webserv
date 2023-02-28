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
_serv(src._serv), _req(src._req), _header(src._header), _raw_env(src._raw_env), _map_cgi(src._map_cgi), _status(0),
_post(false), _get(false) {}

Cgi::Cgi(const Server &serv, const Request &req, Header &header, char **env):
_serv(serv), _req(req), _header(header), _raw_env(env), _map_cgi(serv.getCgi()), _status(0), 
_post(false), _get(false)
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
        this->_get = src._get;
        this->_post = src._post;
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

/*
    Verify if cgi params if set and if the ressource and if the correct extension, 
    if everything match then set _path_cgi to value (<key/extension, value/path_cgi>)
*/
int     Cgi::isCgiRequest(const std::string &path_file)
{
    std::string                                     extension;
    size_t                                          index;
    std::map<std::string, std::string>::iterator    it;

    index = 0;
    this->_map_cgi = this->_serv.getCgi();
    if (!this->_map_cgi.size())
        return (-1);
    it = this->_map_cgi.begin();
    while (it != this->_map_cgi.end())
    {
        extension = ".";
        extension += it->first;
        if ((index = path_file.find(extension)) != (size_t)-1 && !path_file[index + extension.length()])
        {
            this->_path_cgi = it->second;            
            return (1);
        }
        it++;
    }
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

char                **Cgi::exec_args(const std::string &path_file)
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

void                Cgi::setStatus(int s) { this->_status = s; }

void                Cgi::setContentType(const std::string &ct) { this->_header.setContentType(ct); }

void                Cgi::setPoweredBy(const std::string &app) { this->_application = app; }

void                Cgi::setContentLength(const std::string &cl) { this->_header.setContentLength(cl); }

void                Cgi::setCgiErr(const std::string &err) { this->_cgi_err = err; }

void                Cgi::extractFields(const std::string &cgi_response)
{
    std::string                 header;
    std::vector<std::string>    lines;
    std::vector<std::string>    field;
    std::string                 tabFields[5] = 
    {
            "Content-type",
            "Content-length",
            "X-Powered-By",
            "PHP Warning",
            "PHP Parse error" 

    };
    void                (Cgi::*functionPtr[5])(const std::string &) = 
    {
        &Cgi::setContentType,
        &Cgi::setContentLength,
        &Cgi::setCgiErr,
        &Cgi::setCgiErr
    };


    header = cgi_response.substr(0, cgi_response.find("\n\r"));

    lines = ft_split(header, "\n");
    for (size_t i = 0; i < lines.size(); i++)
    {
        field = ft_split(lines[i], ":");

        if (field.size())
        {
            for (size_t j = 0; j < 5; j++)
            {
                if (tabFields[j] == field[0])
                {
                    if (field.size() == 3)
                        (this->*functionPtr[j])(field[1] + field[2]);
                    else if (field[0] == "Status")
                        this->setStatus(ft_stoi(field[1], NULL));
                    else
                        (this->*functionPtr[j])(field[1]);
                }
            }
        }
    }
}

void                child(int fdin, int pipe[2], const std::string &path_script, char **args, char **env)
{
    if (dup2(fdin, 0) == -1)
    {
        perror("dup2 call failed");
        exit(1);
    }
    if (dup2(pipe[1], 1) == -1)
    {
        perror("dup2 call failed");
        exit(1);
    }

    if (execve(path_script.c_str(), args, env) == -1)
    {
        perror("execve call failed");
        close(pipe[1]);
        exit(1);
    }
}

std::string        parent(int pipe[2])
{
    std::string response;
    int         len = 4096;
    char        buff[len];
    int         bytes;

    close(pipe[1]);
    bytes = read(pipe[0], buff, len - 1);
    buff[bytes] = '\0';
    response.append(buff);
//std::cout << response << std::endl;
    while (bytes > 0)
    {
        bytes = read(pipe[0], buff, len - 1);
        buff[bytes] = '\0';
        response.append(buff);
    }

    close(pipe[0]);
    return (response);
}


/*
    GET: set data from env var;
    POST: redirect recv bytes in stdin cgi process
*/

int           Cgi::execute(const std::string &path_file, std::string &body)
{
    int             p[2];
    pid_t           f;

    char            **args;
    char            **env;

    std::string     response;
    std::string     header;
    size_t          index;
    
    int             in;
    
    in = STDIN_FILENO;
    std::cout << "path_file = " << path_file << std::endl;
    env = mapToTab();
    args = exec_args(path_file);

    std::cout << "Cgi: path script " << this->_path_cgi << std::endl; 
    if (pipe(p) == -1)
    {
        perror("pipe call failed");
        return (500);
    }

    if (this->_req.getMethod() == "POST")
    {
        std::cout << "stdin from cgi is a file" << std::endl;
        if ((in = open(path_file.c_str(), 0)) == -1)
        {
            perror("cgi.execute failed can't open path_file");
            return (500);
        }
    }

    f = fork();
    if (f == -1)
    {
        perror("fork call failed");
        return (500);
    }
    if (f == 0)
        child(in, p, this->_path_cgi, args, env);
    else
    {
        // pere
        if (in != STDIN_FILENO)
            close(in);
        int status;

        wait(&status);

        response = parent(p);

         if (!WIFEXITED(status) || (WIFEXITED(status) && WEXITSTATUS(status) != EXIT_SUCCESS))
        {
            std::cout << response << std::endl;
            return (500);
        }

        std::cout << "//////////////////////" << std::endl;
        std::cout << response << std::endl;
        std::cout << "//////////////////////" << std::endl;

        index = response.find("\n\r");

        if (index == (size_t)-1)
        {
            perror("no \\n\\r in cgi response, can't extract header-body: ");
            return (502);
        }

        header = response.substr(0, index);
        body = response.substr(index + 2, response.length());

        std::cout << "///// CGI HEADER  /////" << header << std::endl;
        std::cout << "///// CGI BODY  /////" << body << std::endl;

        extractFields(header);

        std::cout << body.length() << std::endl;

        this->_header.setContentLength(ft_itos(body.length()));

        if ((this->_status == 0 || this->_status == 200) && !this->_cgi_err.length())
            return (200);
    }
    return (500);
}