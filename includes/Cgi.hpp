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

        Server                              _serv;
        Request                             _req;
        Header                              &_header;
        char                                **_raw_env;
        std::map<std::string, std::string>  _env;
        std::map<std::string, std::string>  _map_cgi;

        int                                 _status;
        
        std::string                         _path_cgi;

        std::string                         _body;
        std::string                         _contentType;
        std::string                         _application;
        std::string                         _cgi_err;



        void            addEnvInMap();
        char            **mapToTab();
        char            **exec_args(const std::string &path_file);
        void            extractFields(const std::string &cgi_response);

        void            setStatus(int s);
        void            setContentType(const std::string &ct);
        void            setContentLength(const std::string &ct);
        void            setPoweredBy(const std::string &app);
        void            setCgiErr(const std::string &err);





        

        
};

#endif 

