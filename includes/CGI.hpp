/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   CGI.hpp                                            :+:      :+:    :+:   */
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
#include "Request.hpp"

class CGI
{
    public:

        CGI();
        CGI(const CGI &);
        CGI(const Request &req);
        ~CGI();

        CGI &operator=(const CGI &);


    private:

        int         stdin;
        int         stdout;

        std::string AUTH_TYPE;              // http auth
        std::string CONTENT_LENGHT;
        std::string CONTENT_TYPE;
        std::string GATEWAY_INTERFACE;      // CGI/1.1 gateway review number 
        std::string PATH_INFO;              // path between ressource and query string
        std::string PATH_TRANSLATED;        // path translated from filesystem (/usr/login/Documents/.../file.html)
        std::string QUERY_STRING;
        std::string REMOTE_ADDR;
        std::string REMOTE_HOST;
        std::string REMOTE_IDENT;           // use for identification
        std::string REMOTE_USER;
        std::string REMOTE_MTEHOD;
        std::string SCRIPT_NAME;            // contains th ecurrent script's path;
        std::string SERVER_NAME;
        std::string SERVER_PORT;
        std::string SRVER_PROTOCOL;
        std::string SERVER_SOFTWARE;        // server auth, in http header

        void    initEnv();

};

#endif 

