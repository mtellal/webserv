/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Cgi.cpp                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jdubilla <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/02/13 09:43:33 by mtellal           #+#    #+#             */
/*   Updated: 2023/03/02 10:51:39 by jdubilla         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Cgi.hpp"
#include "utils.hpp"

Cgi::Cgi(const Cgi &src) : _header(src._header) { *this = src; }

Cgi::Cgi(const Server &serv, const Request &req, Header &header, char **env):
_err(false), _get(false), _post(false), 
_rawEnv(env), _req(req), _serv(serv), _header(header)
{
    initEnv();
}

Cgi::~Cgi()
{
    delete [] this->_env;
}

Cgi     &Cgi::operator=(const Cgi &src)
{
    if (this == &src)
    {
        this->_err = src._err;
        this->_get = src._get;
        this->_post = src._post;
        
        this->_env = src._env;
        this->_rawEnv = src._rawEnv;

        this->_body = src._body;
        this->_pathCgiExe = src._pathCgiExe;
        this->_cgiWarnings = src._cgiWarnings;
        
        this->_req = src._req;
        this->_serv = src._serv;
        this->_header = src._header;
        
        this->_envMap = src._envMap;
    }
    return (*this);
}


////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//												G E T T E R													  //
////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void                Cgi::setCgiWarnings(const std::string &err) { this->_cgiWarnings = err; }

void                Cgi::setContentType(const std::string &ct) { this->_header.setContentType(ct); }

void                Cgi::setContentLength(const std::string &cl) { this->_header.setContentLength(cl); }

Header              Cgi::getHeader() const { return (this->_header); }


////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//										M E M B E R S   F U N C T I O N S 									  //
////////////////////////////////////////////////////////////////////////////////////////////////////////////////


void    Cgi::printEnv()
{
    std::map<std::string, std::string>::iterator it = _envMap.begin();

    while (it != _envMap.end())
        std::cout << it->first << " = " << (it++)->second << std::endl;
}

void    Cgi::addVarEnv()
{
    char    **env;
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
        _envMap["CONTENT_LENGTH"] = req.getContentLength();
        if (req.getContentType().length())
            _envMap["CONTENT_TYPE"] = req.getContentType();
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
        errorMessage("new call failed");
        return (NULL);
    }

    while (it != this->_envMap.end())
    {
        std::string  tmp;

        tmp = it->first + "=" + it->second;

        e[i] = strdup(tmp.c_str());

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

void                Cgi::quitChildProccess(int fdin, int p[2], char **args, const std::string &msg) 
{
    (void)args;
    close(fdin);
    errorMessage(msg);
    close(p[1]);
    if (args)
        delete [] args; 
    exit(EXIT_FAILURE);
}

void                Cgi::child(int fdin, int pipe[2], const std::string &file, const std::string &exe)
{
    char **args;

    close(pipe[0]);
    args = execArgs(file, exe);

    if (!args)
        quitChildProccess(fdin, pipe, args, "new call failed (exeArgs)");

    if (fdin != STDIN_FILENO && dup2(fdin, 0) == -1)
        quitChildProccess(fdin, pipe, args, "dup2 call failed");

    if (dup2(pipe[1], 1) == -1)
         quitChildProccess(fdin, pipe, args, "dup2 call failed");
    
    if (execve(args[0], args, this->_env) == -1)
        quitChildProccess(fdin, pipe, args, "execve call failed");
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

char                **Cgi::execArgs(const std::string &file, const std::string &exe)
{
    char        **args;

    args = NULL;
    try
    {
        args = new char*[3];

        args[0] = strdup(exe.c_str());
        args[1] = strdup(file.c_str());
        args[2] = NULL;
    }
    catch (const std::exception &err)
    {
        errorMessage("new call failed (execArgs)");
    }

    return (args);
}

/*
    GET: set data from env var;
    POST: redirect recv bytes in stdin cgi process
*/

void             Cgi::execute(const std::string &file, const std::string &exe, std::string &content)
{
    int             in;
    int             p[2];
    int             status;
    size_t          index;
    std::string     header;
    std::string     response;
    pid_t           f;

    in = STDIN_FILENO;
    if (pipe(p) == -1)
    {
        errorMessage("pipe call failed");
        return (this->_header.setStatus(500));
    }

    if (this->_req.getMethod() == "POST")
    {
        if ((in = open(".bodyfile", 0)) == -1)
            in = STDIN_FILENO;
    }

    f = fork();
    if (f == -1)
    {
        errorMessage("fork call failed");
        return (this->_header.setStatus(500));
    }
    if (f == 0)
        child(in, p, file, exe);
    else
    {
        if (in != STDIN_FILENO)
            close(in);
        waitpid(f, &status, 0);
        response = parent(p);

        if (!WIFEXITED(status) || (WIFEXITED(status) && WEXITSTATUS(status) != EXIT_SUCCESS))
        {
            errorMessage("cgi did not execute correctly");
            return (this->_header.setStatus(502));
        }

        index = response.find("\r\n\r\n");
        if (index == (size_t)-1)
        {
            errorMessage("\\r\\n\\r\\n not found in cgi response");
            return (this->_header.setStatus(502));
        }
        header = response.substr(0, index);
        content = response.substr(index + 4, response.length());
        extractFields(header);
        this->_header.setContentLength(ft_itos(content.length()));
    }
    return ;
}

void			Cgi::errorMessage(const std::string &msg)
{
	time_t		t;
	std::string	_time;

	std::time(&t);
	_time = std::ctime(&t);
	std::cerr << "\033[1;34m[" << _time.substr(0, _time.length() - 1) << "]\033[0m";
	std::cerr << "\033[1;36m [RESPONSE] [CGI]\033[0m";
	std::cerr << "\033[1;31m [ERROR] \033[0m";
	std::cerr << msg << std::endl;
}