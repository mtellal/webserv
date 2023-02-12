#ifndef HEADER_HPP
# define HEADER_HPP

# include "Request.hpp"
# include "Server.hpp"
# include "Response.hpp"
# include <string>

class Response;

class Header {

	public:
	Header();
	Header(Request req, std::string file, int *statusCode, Server serv, Response *rep);
	Header(Header const &src);
	~Header();

	Header	&operator=(Header const &rhs);

	std::string	getHeader() const;


	private:
	Request		_req;
	int			*_statusCode;
	std::string	_file;
	Server		_serv;
	Response	*_rep;

	std::string	ft_itos(int nbr) const;
	std::string	getContentType() const;
	std::string	getDate() const;
	std::string	getContentLength() const;
	std::string	getCodeDescription() const;
	std::string	getLastModified() const;
	std::string	getAllow() const;

};

#endif