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

CGI::CGI() : stdin(0), stdout(1)
{
}

CGI::CGI(const CGI &src) : stdin(src.stdin), stdout(src.stdout)
{
}

CGI::~CGI()
{
}

/* void    CGI::initEnv(const Request &req)
{
    AUTH_TYPE           =   req.getAuthentification();
    CONTENT_LENGTH      =   req.getContentLength();
    CONTENT_TYPE        =   req.getContentType();
    GATEWAY_INTERFACE   =   "CGI/1.1";
    PATH_INFO           =   "";

} */
