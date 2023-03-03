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
        
        int             isCgiRequest(const std::string &path_file);
        void            initEnv();
        void            printEnv();
        int             execute(const std::string &path_file, std::string &body);

        Header          getHeader() const;

    private:

        bool                                _post;
        bool                                _get;
        bool                                _err;

        char                                **_rawEnv;
        char                                **_env;
        std::string                         _pathCgiExe;

        std::string                         _body;
        std::string                         _cgiWarnings;

        Server                              _serv;
        Request                             _req;
        Header                              &_header;

        std::map<std::string, std::string>  _envMap;
        


        void            addVarEnv();
        void            addCgiVarEnv();
        char            **mapToTab();
        char            **exec_args(const std::string &path_file);
        void            extractFields(const std::string &cgi_response);

        void            setStatus(int s);
        void            setContentType(const std::string &ct);
        void            setContentLength(const std::string &ct);
        void            setPoweredBy(const std::string &app);
        void            setCgiWarnings(const std::string &err);

        void            child(int fdin, int pipe[2], const std::string &path_script, char **args);






        

        
};

#endif 

