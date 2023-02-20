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
#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <errno.h>
#include <cstdlib>
#include <string.h>


class CGI
{
    public:

        CGI();
        CGI(const CGI &);
        CGI(const Request &req, char **env);
        ~CGI();

        CGI &operator=(const CGI &);
        
        std::string    execute(const std::string &path, char **env);
        void            initEnv(const Request &req, char **env);
        void            printEnv();


    private:

        int                                 _stdin;
        int                                 _stdout;
        std::map<std::string, std::string>  _env;
        

        
};

#endif 

