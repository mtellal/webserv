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


#ifndef Cgi_HPP
#define Cgi_HPP

#include <string>
#include "Request.hpp"
#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <errno.h>
#include <cstdlib>
#include <string.h>


class Cgi
{
    public:

        Cgi();
        Cgi(const Cgi &);
        Cgi(const Request &req, char **env);
        ~Cgi();

        Cgi &operator=(const Cgi &);
        
        void            initEnv(const Request &req, char **env);
        std::string     execute(const std::string &path_cgi, const std::string &path_file);
        void            printEnv();


    private:

        int                                 _stdin;
        int                                 _stdout;
        std::map<std::string, std::string>  _env;

        void            addEnvInMap(char **env);
        char            **mapToTab();

        

        
};

#endif 

