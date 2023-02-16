/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mtellal <marvin@42.fr>                     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/02/16 11:12:14 by mtellal           #+#    #+#             */
/*   Updated: 2023/02/16 11:14:52 by mtellal          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Client.hpp"

Client::Client() : fd(-1)
{
	memset(&addr, 0, sizeof(addr));
}

Client::Client(const Client &c)
{
	*this = c;
}

Client	&Client::operator=(const Client &c)
{
	if (this != &c)
	{
		ip_address = c.ip_address;
		port = c.port;
		fd = c.fd;
	}
	return (*this);
}

Client::~Client() {}

std::string         Client::getIpAddress() const { return (ip_address); }

std::string         Client::getPort() const { return (port); }

int                 Client::getFD() const { return (fd); }

struct sockaddr     Client::getAddr() const { return (addr); }

void				Client::set(std::string ip, std::string p, int f, struct sockaddr &addr) 
{
	setIpAddress(ip);
	setPort(p);
	setFD(f);
	setAddr(addr);
}

void                Client::setIpAddress(std::string s) { ip_address = s; }

void                Client::setPort(std::string s) { port = s; }

void                Client::setFD(int i) { fd = i; }

void                Client::setAddr(struct sockaddr &a) { addr = a; }

std::ostream &operator<<(std::ostream &out, const Client &c)
{
	out << "Client [" << c.getFD() << "] 's informations: \n";
	out << "ip: " << c.getIpAddress() << "\n";
	out << "port: " << c.getPort() << "\n";
	out << "fd: " << c.getFD() << "\n";
	return (out);
}