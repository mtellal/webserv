/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   CGI.cpp                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mtellal <marvin@42.fr>                     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/02/13 09:43:33 by mtellal           #+#    #+#             */
/*   Updated: 2023/02/13 09:43:35 by mtellal          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "CGI.hpp"

CGI::CGI() : _stdin(0), _stdout(1) {}

CGI::CGI(const CGI &src) : _stdin(src._stdin), _stdout(src._stdout) {}

CGI::CGI(const Request &req, char **env)
{
    (void)req;
    (void)env;
    initEnv(req, env);
    std::cout << _env["PATH_INFO"] << std::endl;
}

CGI::~CGI() {}

size_t    tab_len(char **env)
{
    size_t  i = 0;
    
    while (env && env[i])
        i++;
    return (i);
}

void    CGI::printEnv()
{
    std::map<std::string, std::string>::iterator it = _env.begin();

    while (it != _env.end())
        std::cout << it->first << " = " << (it++)->second << std::endl;
}

void    CGI::initEnv(const Request &req, char **env)
{
    _env["AUTH_TYPE"]           =   req.getAuthentification();
    _env["CONTENT_LENGTH"]      =   req.getContentLength();
    _env["CONTENT_TYPE"]        =   req.getContentType();
    _env["GATEWAY_INTERFACE"]   =   "CGI/1.1";
    _env["PATH_INFO"]           =   req.getPath();
    _env["PATH_TRANSLATED"]     =   req.getPath();
    _env["QUERY_STRING"]        =   "";
    _env["REMOTE_ADDR"]         =   ""; // need to be set
    _env["REMOTE_HOST"]         =   ""; // NULL
    _env["REMOTE_IDENT"]        =   ""; // NULL
    _env["REMOTE_USER"]         =   ""; // NULL
    _env["REMOTE_METHOD"]       =   req.getMethod();
    _env["SCRIPT_NAME"]         =   req.getPath();
    _env["SERVER_NAME"]         =   "";
    _env["SERVER_PORT"]         =   req.getPort();
    _env["SERVER_PROTOCOL"]     =   "HTTP/1.1";
    _env["SERVER_SOFTWARE"]     =   req.getServerName();

    size_t len = tab_len(env);

    while (len > 0)
    {
        std::string var_env;
        size_t      index;
        std::string key;
        std::string value;

        var_env = env[len - 1];
        index = var_env.find("=");
        key = var_env.substr(0, index);
        value = var_env.substr(index + 1, var_env.length());
        _env[key] = value;
        len--;
    }

}

std::string   CGI::execute(const std::string &path, char **env)
{
    int     p[2];
    pid_t   f;

    (void)path;

    if (pipe(p) == -1)
    {
        perror("pipe call failed");
        return ("error");
    }

    f = fork();
    if (f == -1)
        perror("fork call failed");
    if (f == 0)
    {
        // child
        close(p[0]);
        if (dup2(p[1], 1) == -1)
        {
            perror("dup2 call failed");
            return ("error");
        }

        char **args = new char*[2];
        args[0] = strdup("/usr/bin/php-cgi");
        args[1] = strdup("./html/test.php");

        if (execve(path.c_str(), args, env) == -1)
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
        return (respond);
    }
    return ("error");
}