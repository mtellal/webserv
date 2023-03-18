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


Cgi::Cgi(const Server &serv, const Request &req, char **env):
_err(false), _get(false), _post(false), _infile(STDIN_FILENO)
{
    initEnv(serv, req, env);

    if (req.getMethod() == "POST")
    {
        if ((_infile = open(".bodyfile", 0)) == -1)
        {
            _infile = STDIN_FILENO;
        }
    }
}

Cgi::Cgi(const Cgi &src) { *this = src; }

Cgi::~Cgi()
{
}

Cgi     &Cgi::operator=(const Cgi &src)
{
    if (this == &src)
    {
        this->_err = src._err;
        this->_get = src._get;
        this->_post = src._post;
        
        this->_env = src._env;

        this->_body = src._body;
        this->_pathCgiExe = src._pathCgiExe;
        this->_cgiWarnings = src._cgiWarnings;
        
        this->_envMap = src._envMap;
    }
    return (*this);
}


////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//												G E T T E R													  //
////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void                Cgi::setCgiWarnings(const std::string &err) { this->_cgiWarnings = err; }

std::string         Cgi::getContentType() const { return (this->_contentType); }


////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//										M E M B E R S   F U N C T I O N S 									  //
////////////////////////////////////////////////////////////////////////////////////////////////////////////////


void    Cgi::printEnv()
{
    std::map<std::string, std::string>::iterator it = _envMap.begin();

    while (it != _envMap.end())
        std::cout << it->first << " = " << (it++)->second << std::endl;
}

void    Cgi::addVarEnv(char **rawEnv)
{
    size_t  i;
    size_t  len;

    i = 0;
    len = tab_len(rawEnv);
    while (i < len)
    {
        std::string var_envMap;
        size_t      index;
        std::string key;
        std::string value;

        var_envMap = rawEnv[i];
        index = var_envMap.find("=");
        key = var_envMap.substr(0, index);
        value = var_envMap.substr(index + 1, var_envMap.length());
        _envMap[key] = value;
        i++;
    }
}

void    Cgi::addCgiVarEnv(const Server &serv, const Request &req)
{
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
    _envMap["SCRIPT_NAME"]         =  "." + serv.getRoot() + req.getPath();
    _envMap["SCRIPT_FILENAME"]     =   "." + serv.getRoot() + req.getPath();
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
   
    e = (char**)malloc(sizeof(char *) * (this->_envMap.size() + 1));

    if (!e)
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

void    Cgi::initEnv(const Server &serv, const Request &req, char **rawEnv)
{
    addCgiVarEnv(serv, req);

    addVarEnv(rawEnv);

    this->_env = mapToTab();

    if (!this->_env)
    {
        //this->_header.setStatus(500);
        
        this->_err = true;
    }
}

void                Cgi::quitChildProccess(int fdin, int p[2], char **args, const std::string &msg) 
{
    close(_infile);
    close(fdin);
    errorMessage(msg);
    close(p[1]);
    free_tab(args);
    free_tab(this->_env);
    _envMap.clear();
    exit(EXIT_FAILURE);
}

char                **Cgi::execArgs(const std::string &file, const std::string &exe)
{
    char        **args;

    args = NULL;
    
    args = (char **)malloc(sizeof(char *) * 3);

    if (!args)
        errorMessage("new call failed (execArgs)");

    args[0] = strdup(exe.c_str());
    args[1] = strdup(file.c_str());

    args[2] = NULL;

    return (args);
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
    
    execve(args[0], args, this->_env);
    quitChildProccess(fdin, pipe, args, "execve call failed");
}

std::string        parent(int pipe[2], int infile)
{
    std::string response;
    int         len = 4096;
    char        buff[len];
    int         bytes;

    close(infile);
    close(pipe[1]);
    bytes = read(pipe[0], buff, len - 1);
    if (bytes < 1)
    {
        close(pipe[0]);
        return (response);
    }
    buff[bytes] = '\0';
    response.append(buff);
    while (bytes > 0)
    {
        bytes = read(pipe[0], buff, len - 1);
        if (bytes < 1)
        {
            close(pipe[0]);
            return (response);
        }
        buff[bytes] = '\0';
        response.append(buff);
    }
    close(pipe[0]);
    return (response);
}

void                Cgi::extractContentType(const std::string &cgi_header)
{
    std::string                 header;
    std::vector<std::string>    lines;
    std::vector<std::string>    field;

    header = cgi_header.substr(0, cgi_header.find("\n\r"));

    lines = ft_split(header, "\n");
    for (size_t i = 0; i < lines.size(); i++)
    {
        field = ft_split(lines[i], ":");

        if (field.size())
        {
            for (size_t j = 0; j < 5; j++)
            {
                if ("Content-type" == field[0])
                    this->_contentType = field[1];
            }
        }
    }
}

int             verifExePath(const std::string &exe)
{
    struct stat file;
    mode_t      t;
    
    memset(&file, 0, sizeof(file));
    stat(exe.c_str(), &file);
    if (!S_ISREG(file.st_mode))
        return (-1);
    t = file.st_mode;
    t = t << 5;
    t = t >> 5;
    if (!(t & S_IXUSR))
        return (-1);
    return (0);
}

int             Cgi::execute(const std::string &file, const std::string &exe, std::string &content)
{
    int             p[2];
    int             status;
    size_t          index;
    std::string     response;
    pid_t           f;
    time_t          begin;
    time_t          end;
    
    time(&begin);
    if (pipe(p) == -1)
    {
        free_tab(this->_env);
        errorMessage("pipe call failed");
        return (500);
    }

    if (verifExePath(exe) == -1)
    {
        free_tab(this->_env);
        errorMessage(exe + " can't be executed");
        return (200);
    }

    f = fork();
    if (f == -1)
    {
        free_tab(this->_env);
        errorMessage("fork call failed");
        return (500);
    }
    if (f == 0)
        child(_infile, p, file, exe);
    else
    {
        free_tab(this->_env);
        if (_infile != STDIN_FILENO)
            close(_infile);
        while (waitpid(f, &status, WNOHANG) == 0)
        {
            time(&end);
            sleep(1);
            if (difftime(end, begin) > 5)
            {
                errorMessage("timeout cgi");
                kill(f, 0);
                return (504);
            }
        }

        response = parent(p, _infile);
        if (!WIFEXITED(status) || (WIFEXITED(status) && WEXITSTATUS(status) != EXIT_SUCCESS))
        {
            errorMessage("cgi did not execute correctly EXIT_FAILURE");
            return (502);
        }

        index = response.find("\r\n\r\n");
        if (index == (size_t)-1)
            errorMessage("\\r\\n\\r\\n not found in cgi response");
        else
        {
            extractContentType(response.substr(0, index));
            content = response.substr(index + 4, response.length());
        }
    }
    return (200);
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