/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jdubilla <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/02/16 11:15:04 by mtellal           #+#    #+#             */
/*   Updated: 2023/03/08 13:09:46 by jdubilla         ###   ########.fr       */
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

		void			setFD(int i);
		void			setIpAddress(std::string s);
		void			setAddr(struct sockaddr &addr);
		void			set(std::string ip, int f, struct sockaddr &addr);

		int getFD()		const;
		std::string		getIpAddress() const;
		struct sockaddr	getAddr() const;

	private:
		int				fd;
		std::string		ip_address;
		struct sockaddr	addr;
};

std::ostream &operator<<(std::ostream &out, const Client &c);

#endif
