/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mtellal <marvin@42.fr>                     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/02/16 11:15:04 by mtellal           #+#    #+#             */
/*   Updated: 2023/02/16 11:15:29 by mtellal          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CLIENT_HPP
#define CLIENT_HPP

#include <string.h>
#include <iostream>
#include <sys/socket.h>
#include <netdb.h>

class Client 
{

    public:

        Client();
        Client(const Client &c);
        ~Client();

        Client &operator=(const Client &c);

        std::string         getIpAddress() const;
        std::string         getPort() const;
        int                 getFD() const;
        struct sockaddr     getAddr() const;
    
        void				set(std::string ip, std::string p, int f, struct sockaddr &addr);
        void                setIpAddress(std::string s);
        void                setPort(std::string s);
        void                setFD(int i);
        void                setAddr(struct sockaddr &addr);

    private:

        std::string         ip_address;
        std::string         port;
        int                 fd;
        struct sockaddr     addr;

};

std::ostream &operator<<(std::ostream &out, const Client &c);

#endif
