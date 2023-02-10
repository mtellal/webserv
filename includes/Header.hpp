#ifndef HEADER_HPP
# define HEADER_HPP

# include "Request.hpp"
# include <string>

class Header {

	public:
	Header();
	Header(Request req, std::string file, int *statusCode);
	Header(Header const &src);
	~Header();

	Header	&operator=(Header const &rhs);

	std::string	getHeader() const;


	private:
	Request		_req;
	int			*_statusCode;
	std::string	_file;

	std::string	ft_itos(int nbr) const;
	std::string	getContentType() const;
	std::string	getDate() const;
	std::string	getContentLength() const;
	std::string	getCodeDescription() const;

};

#endif