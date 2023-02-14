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

CGI::CGI(const Request &req)
{
    (void)req;
   // initEnv(req);
}

CGI::~CGI() {}

void    CGI::initEnv(const Request &req)
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
}

#include <string.h>
#include <sys/wait.h>

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

        char **args = (char**)malloc(sizeof(char**) * 2);
        args[0] = strdup("/usr/bin/php-cgi");
        args[1] = strdup("/mnt/nfs/homes/mtellal/Documents/42-webserv/html/test.php");

        std::cerr << "execve" << std::endl;
        if (execve("/usr/bin/php-cgi", args, env) == -1)
        {
            std::cerr << "error avec execve" << std::endl;
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

        while (bytes > 1)
        {
            bytes = read(p[0], buff, len - 1);
            buff[bytes] = '\0';
            respond.append(buff);
        }

        std::cout << respond << std::endl;
        close(p[0]);

        return (respond);
    }
    return ("error");
}