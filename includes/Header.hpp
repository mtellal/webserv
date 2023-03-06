#ifndef HEADER_HPP
# define HEADER_HPP

# include <sstream>
# include <sys/stat.h>
# include <iostream>
# include <fstream>
# include <string>
# include <stdio.h>
# include <ctime>

# include "Request.hpp"
# include "Server.hpp"
# include "Response.hpp"
# include "utils.hpp"

class Response;

class Header {

	public:

		Header();
		Header(std::string file, int *statusCode, Response *rep);
		Header(std::string file, int *statusCode);
		Header(Header const &src);
		~Header();
	
		Header	&operator=(Header const &rhs);

		void		setStatus(int status);
		void		setContentType(std::string const &contentType);
		void		setContentLength(std::string const &contentLength);

		std::string	getHeader();
		std::string	getContentType();
		std::string	getHeaderRequestError();


	private:
	
		int									*_statusCode;
		std::string							_file;
		Request								_req;
		Server								_serv;
		Response							*_rep;
		std::map<std::string, std::string>	_header;

		std::string	getDate() const;
		std::string	getAllow() const;
		std::string	ft_itos(int nbr) const;
		std::string	getLastModified() const;
		std::string	getContentLength() const;
		std::string	parsContentTypeFile(std::vector<std::string> splitAccept) const;

};

#endif