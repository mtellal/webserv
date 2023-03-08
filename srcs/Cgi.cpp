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

Cgi::~Cgi() {}

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
        _envMap["CONTENT_LENGTH"] = req.getContentLength();
        std::cout << _envMap["CONTENT_LENGTH"] << std::endl;
        if (req.getContentType().length())
        {
            _envMap["CONTENT_TYPE"] = req.getContentType();
            std::cout << _envMap["CONTENT_TYPE"] << std::endl;
        }
        else 
            _envMap["CONTENT_TYPE"] = "";
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

void                Cgi::child(int fdin, int pipe[2], char **args)
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

    if (execve(args[0], args, this->_env) == -1)
    {
        perror("execve call failed");
        std::cerr << "exe " << args[0] << std::endl;
        std::cerr << "file " << args[1] << std::endl;
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
        std::cerr << " new call failed (exec_args)" << std::endl;
    }

    return (args);
}

void        Cgi::quitCgi(int status)
{
    this->_header.setStatus(status);
}

/*
    GET: set data from env var;
    POST: redirect recv bytes in stdin cgi process
*/

void             Cgi::execute(const std::string &file, const std::string &exe, std::string &content)
{
    int             in;
    int             p[2];
   // int             status;
    char            **args;
    size_t          index;
    std::string     header;
    std::string     response;
    pid_t           f;

    
    std::cout << "///////////// C G I    E X E C U T E   C A L L E D ///////////////" << std::endl;


    in = STDIN_FILENO;
    args = execArgs(file, exe);
    if (pipe(p) == -1)
    {
        perror("pipe call failed");
        return (quitCgi(500));
    }

    if (this->_req.getMethod() == "POST")
    {
        std::cout << "stdin from cgi is a file" << std::endl;
        if ((in = open(".bodyfile", 0)) == -1)
        {
            perror("cgi.execute failed can't open path_file");
            return (quitCgi(500));
        }
    }

    std::cout << "file: " << file << std::endl;
    std::cout << "exe: " << exe << std::endl;

    std::cout << "in: " << in << std::endl;
    std::cout << "method: " << _req.getMethod() << std::endl;

    f = fork();
    if (f == -1)
    {
        perror("fork call failed");
        return (quitCgi(500));
    }
    if (f == 0)
        child(in, p, args);
    else
    {
        // pere
        if (in != STDIN_FILENO)
            close(in);
        
        /* waitpid(f, &status, 0);
        
        if (!WIFEXITED(status) || (WIFEXITED(status) && WEXITSTATUS(status) != EXIT_SUCCESS))
            return (quitCgi(502)); */

        response = parent(p);

        std::cout << "//////////////////////" << std::endl;
        std::cout << response << std::endl;
        std::cout << "//////////////////////" << std::endl;

        index = response.find("\r\n\r\n");

        if (index == (size_t)-1)
        {
            perror("no \\r\\n\\r\\n in cgi response, can't extract header-body: ");
            return (quitCgi(502));
        }

        header = response.substr(0, index);
        content = response.substr(index + 4, response.length());
        extractFields(header);
        this->_header.setContentLength(ft_itos(content.length()));
    }
    return ;
}