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
		fd = c.fd;
		addr = c.addr;
	}
	return (*this);
}

Client::~Client() {}

std::string         Client::getIpAddress() const { return (ip_address); }

int                 Client::getFD() const { return (fd); }

struct sockaddr     Client::getAddr() const { return (addr); }

void				Client::set(std::string ip, int f, struct sockaddr &addr) 
{
	setIpAddress(ip);
	setFD(f);
	setAddr(addr);
}

void                Client::setIpAddress(std::string s) { ip_address = s; }

void                Client::setFD(int i) { fd = i; }

void                Client::setAddr(struct sockaddr &a) { addr = a; }

std::ostream &operator<<(std::ostream &out, const Client &c)
{
	out << "Client [" << c.getFD() << "] 's informations: \n";
	out << "ip: " << c.getIpAddress() << "\n";
	out << "fd: " << c.getFD() << "\n";
	return (out);
}