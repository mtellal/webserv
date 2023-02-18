/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   DefaultPage.hpp                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mtellal <marvin@42.fr>                     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/02/18 18:19:48 by mtellal           #+#    #+#             */
/*   Updated: 2023/02/18 18:20:25 by mtellal          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef DEFAULT_PAGES_HPP
#define DEFAULT_PAGES_HPP

#include <fstream>
#include <iostream>
#include <sys/stat.h>
#include <dirent.h>

#include "Request.hpp"
#include "Server.hpp"

class	DefaultPage
{
    public:

        DefaultPage();
        DefaultPage(const DefaultPage &pages);
        DefaultPage(const Request &r, const Server &s);
        ~DefaultPage();

        DefaultPage    &operator=(const DefaultPage &pages);

        std::string     createResFormPage();
        std::string     createDefaultErrorPage(int statusCode);
        std::string     createAutoindexPage(std::vector<std::string> p);


    private:

        Request         _req;
        Server          _serv;

        void            fileAndDir(std::ofstream &file, bool getDir, std::string path);
        std::string		argsToStr();

};

#endif
