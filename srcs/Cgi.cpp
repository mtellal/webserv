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

Cgi::Cgi(const Cgi &src) : _header(src._header) { *this = src; }

Cgi::Cgi(const Server &serv, const Request &req, Header &header, char **env):
_post(false), _get(false), _err(false),
_rawEnv(env), _env(NULL), _pathCgiExe(""), _body(""), _cgiWarnings(""),
_serv(serv), _req(req), _header(header) 
{
    initEnv();
}

Cgi::~Cgi() {}

Cgi     &Cgi::operator=(const Cgi &src)
{
    if (this == &src)
    {
        this->_get = src._get;
        this->_post = src._post;
        this->_err = src._err;
        this->_rawEnv = src._rawEnv;
        this->_envMap = src._envMap;


        this->_pathCgiExe = src._pathCgiExe;
        this->_body = src._body;
        this->_cgiWarnings = src._cgiWarnings;

        this->_serv = src._serv;
        this->_req = src._req;
        this->_header = src._header;
    }
    return (*this);
}

Header          Cgi::getHeader() const { return (this->_header); }

/*
    Verify if cgi params if set and if the ressource and if the correct extension, 
    if everything match then set _pathCgiExe to value (<key/extension, value/path_cgi>)
*/
int     Cgi::isCgiRequest(const std::string &path_file)
{
    std::string                                     extension;
    size_t                                          index;
    std::map<std::string, std::string>::iterator    it;
    std::map<std::string, std::string>           mapConfCgi;

    index = 0;
    mapConfCgi = this->_serv.getCgi();
    if (!mapConfCgi.size())
        return (-1);
    it = mapConfCgi.begin();
    std::cout << "Cgi: map_cgi.size() " << mapConfCgi.size() << std::endl;
    while (it != mapConfCgi.end())
    {
        std::cout << "Cgi: map[i]: " << it->first << " " << it->second << std::endl;
        extension = ".";
        extension += it->first;
        if ((index = path_file.find(extension)) != (size_t)-1 && !path_file[index + extension.length()])
        {
            this->_pathCgiExe = it->second;            
            return (1);
        }
        it++;
    }
    return (-1);
}


void    Cgi::printEnv()
{
    std::map<std::string, std::string>::iterator it = _envMap.begin();

    while (it != _envMap.end())
        std::cout << it->first << " = " << (it++)->second << std::endl;
}

void    Cgi::addVarEnv()
{
    char    ** env;
    size_t  i;
    size_t  len;

    i = 0;
    env = this->_rawEnv;
    len = tab_len(env);
    while (i < len)
    {
        std::string var_envMap;
        size_t      index;
        std::string key;
        std::string value;

        var_envMap = env[i];
        index = var_envMap.find("=");
        key = var_envMap.substr(0, index);
        value = var_envMap.substr(index + 1, var_envMap.length());
        _envMap[key] = value;
        i++;
    }
}

void    Cgi::addCgiVarEnv()
{
    Request &req = this->_req;

    _envMap["REDIRECT_STATUS"]     =   "200";
    _envMap["AUTH_TYPE"]           =   req.getAuthentification();
    if (req.getMethod() == "POST")
    {
        _envMap["CONTENT_LENGTH"] = "618";
        if (req.getContentType().length())
        {
            std::cout << "content type set" << std::endl;
            _envMap["CONTENT_TYPE"] = "multipart/form-data;boundary=------WebKitFormBoundary3IMBGwkS5BuesHoI";
        }
    }
    else
        _envMap["QUERY_STRING"]        =   req.getQueryString();
    _envMap["GATEWAY_INTERFACE"]   =   "Cgi/1.1";
    _envMap["PATH_INFO"]           =   req.getPath();
    _envMap["PATH_TRANSLATED"]     =   req.getPath();
    _envMap["REMOTE_ADDR"]         =   ""; // need to be set
    _envMap["REMOTE_HOST"]         =   ""; // NULL
    _envMap["REMOTE_IDENT"]        =   ""; // NULL
    _envMap["REMOTE_USER"]         =   ""; // NULL
    _envMap["REQUEST_METHOD"]       =   req.getMethod();
    _envMap["SCRIPT_NAME"]         =  "." + _serv.getRoot() + req.getPath();
    _envMap["SCRIPT_FILENAME"]     =   "." + _serv.getRoot() + req.getPath();
    _envMap["SERVER_NAME"]         =   req.getServerName();
    _envMap["SERVER_PORT"]         =   req.getPort();
    _envMap["SERVER_PROTOCOL"]     =   "HTTP/1.1";
    _envMap["SERVER_SOFTWARE"]     =   req.getServerName();
}

char    **Cgi::mapToTab()
{
    size_t                                          i;
    char                                            **e;
    std::map<std::string, std::string>::iterator    it;

    i = 0;
    it = this->_envMap.begin();
    try
    {
        e = new char*[this->_envMap.size() + 1];
    }
    catch (const std::exception &err)
    {
        std::cerr << "(mapToTab) creation of env general failed: " << err.what() << std::endl;
        return (NULL);
    }

    while (it != this->_envMap.end())
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
            while (i > 0)
            {
                i--;
                delete e[i];
            }
            return (NULL);
        }

        strcpy(e[i], tmp.c_str());
        e[i][tmp.length()] = '\0';
        i++;
        it++;
    }
    e[i] = NULL;
    return (e);
}

/*
    add predefined cgi variables, and default env variables in a map (_envMap)
*/

void    Cgi::initEnv()
{
    addCgiVarEnv();

    addVarEnv();

    this->_env = mapToTab();

    if (!this->_env)
    {
        this->_header.setStatus(500);
        this->_err = true;
    }
}

char                **Cgi::exec_args(const std::string &path_file)
{
    char        **args;

    try
    {
        args = new char*[3];

        args[0] = strdup(this->_pathCgiExe.c_str());
        args[1] = strdup(path_file.c_str());
        args[2] = NULL;
    }
    catch (const std::exception &err)
    {
        std::cerr << " new call failed (exec_args)" << std::endl;
    }

    return (args);
}

void                Cgi::setContentType(const std::string &ct) { this->_header.setContentType(ct); }

void                Cgi::setContentLength(const std::string &cl) { this->_header.setContentLength(cl); }

void                Cgi::setCgiWarnings(const std::string &err) { this->_cgiWarnings = err; }

void                Cgi::extractFields(const std::string &cgi_response)
{
    std::string                 header;
    std::vector<std::string>    lines;
    std::vector<std::string>    field;
    std::string                 tabFields[5] = 
    {
            "Content-type",
            "Content-length",
            "PHP Warning",
            "PHP Parse error" 

    };
    void                (Cgi::*functionPtr[5])(const std::string &) = 
    {
        &Cgi::setContentType,
        &Cgi::setContentLength,
        &Cgi::setCgiWarnings,
        &Cgi::setCgiWarnings
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
                    else if (field.size() > 1 && field[0] == "Status")
                        this->_header.setStatus(ft_stoi(field[1], NULL));
                    else
                        (this->*functionPtr[j])(field[1]);
                }
            }
        }
    }
}

void                Cgi::child(int fdin, int pipe[2], const std::string &path_script, char **args)
{
    if (fdin != STDIN_FILENO && dup2(fdin, 0) == -1)
    {
        perror("dup2 call failed");
        exit(1);
    }

    if (dup2(pipe[1], 1) == -1)
    {
        perror("dup2 call failed");
        exit(1);
    }

    close(pipe[0]);

    if (execve(path_script.c_str(), args, this->_env) == -1)
    {
        std::cerr << "execve failed" << std::endl;
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

    std::string     response;
    std::string     header;
    size_t          index;
    
    int             in;
    
    in = STDIN_FILENO;
    args = exec_args(path_file);

    std::cout << "Cgi: path script " << this->_pathCgiExe << std::endl; 

    std::cout << "///////////// C G I    E X E C U T E   C A L L E D ///////////////" << std::endl;


    if (pipe(p) == -1)
    {
        perror("pipe call failed");
        return (500);
    }

    if (this->_req.getMethod() == "POST")
    {
        std::cout << "stdin from cgi is a file" << std::endl;
        if ((in = open("./uploads/inputCGI", 0)) == -1)
        {
            perror("cgi.execute failed can't open path_file");
            return (500);
        }
    }

    std::cout << "path_file: " << path_file << std::endl;
    std::cout << "in: " << in << std::endl;
    std::cout << "method: " << _req.getMethod() << std::endl;
    std::cout << "path: " << path_file << std::endl;


    f = fork();
    if (f == -1)
    {
        perror("fork call failed");
        return (500);
    }
    if (f == 0)
        child(in, p, this->_pathCgiExe, args);
    else
    {
        // pere
        if (in != STDIN_FILENO)
            close(in);
        int status;

        waitpid(f, &status, -1);

        response = parent(p);


         if (!WIFEXITED(status) || (WIFEXITED(status) && WEXITSTATUS(status) != EXIT_SUCCESS))
        {
            //std::cout << response << std::endl;
            return (500);
        }

        std::cout << "//////////////////////" << std::endl;
        std::cout << response.substr(0, 100) << std::endl;
        std::cout << "//////////////////////" << std::endl;

        index = response.find("\n\r");

        if (index == (size_t)-1)
        {
            perror("no \\n\\r in cgi response, can't extract header-body: ");
            return (502);
        }

        header = response.substr(0, index);
        body = response.substr(index + 2, response.length());

       /*  std::cout << "///// CGI HEADER  /////" << header << std::endl;
        std::cout << "///// CGI BODY  /////" << body << std::endl; */

        extractFields(header);

        std::cout << body.length() << std::endl;

        this->_header.setContentLength(ft_itos(body.length()));

    }
    return (0);
}