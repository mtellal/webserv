/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Cgi.hpp                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mtellal <marvin@42.fr>                     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/02/13 09:43:57 by mtellal           #+#    #+#             */
/*   Updated: 2023/02/13 09:43:59 by mtellal          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */


#ifndef CGI_HPP
#define CGI_HPP

#include <string>
#include <stdio.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <errno.h>
#include <cstdlib>
#include <string.h>
#include <fcntl.h>

#include "Request.hpp"
#include "Server.hpp"
#include "Header.hpp"
#include "utils.hpp"

class Cgi
{
    public:

        Cgi(const Cgi &);
        Cgi(const Server &serv, const Request &req, char **env);
        ~Cgi();

        Cgi &operator=(const Cgi &);
        
        int             execute(const std::string &file, const std::string &exe, std::string &content);
        bool            getErrInit() const;
        std::string     getContentType() const;

    private:

        bool                                _errInit;
        bool                                _get;
        bool                                _post;

        char                                **_env;

        int                                 _infile;

        std::string                         _body;
        std::string                         _pathCgiExe;
        std::string                         _contentType;
        std::string                         _cgiWarnings;


        std::map<std::string, std::string>  _envMap;
        
        void            printEnv();
        void            setStatus(int s);
        void            addVarEnv(char **rawEnv);
        void			errorMessage(const std::string &msg);
        void            setPoweredBy(const std::string &app);
        void            setContentType(const std::string &ct);
        void            setCgiWarnings(const std::string &err);
        void            setContentLength(const std::string &ct);
        void            extractContentType(const std::string &cgi_response);
        void            addCgiVarEnv(const Server &serv, const Request &req);
        void            initEnv(const Server &serv, const Request &req, char **rawEnv);
        void            quitChildProccess(int fdin, int p[2], char **args, const std::string &msg);
        void            child(int fdin, int pipe[2], const std::string &file, const std::string &exe);
        
        char            **mapToTab();
        char            **execArgs(const std::string &file, const std::string &exe);

        
};

#endif 

