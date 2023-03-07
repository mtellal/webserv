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
        Cgi(const Server &serv, const Request &req, Header &header, char **env);
        ~Cgi();

        Cgi &operator=(const Cgi &);
        
        void            initEnv();
        void            printEnv();
        void            execute(const std::string &file, const std::string &exe, std::string &content);

        Header          getHeader() const;

    private:

        bool                                _err;
        bool                                _get;
        bool                                _post;

        char                                **_env;
        char                                **_rawEnv;

        std::string                         _body;
        std::string                         _pathCgiExe;
        std::string                         _cgiWarnings;

        Request                             _req;
        Server                              _serv;
        Header                              &_header;

        std::map<std::string, std::string>  _envMap;
        


        void            addVarEnv();
        void            addCgiVarEnv();
        void            setStatus(int s);
        void            quitCgi(int status);
        void            setPoweredBy(const std::string &app);
        void            setContentType(const std::string &ct);
        void            setCgiWarnings(const std::string &err);
        void            setContentLength(const std::string &ct);
        void            child(int fdin, int pipe[2], char **args);
        void            extractFields(const std::string &cgi_response);
        
        char            **mapToTab();
        char            **execArgs(const std::string &file, const std::string &exe);
        
};

#endif 

