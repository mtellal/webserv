#ifndef HEADER_HPP
# define HEADER_HPP

# include <string>

class Header {

	public:
	Header();
	Header(std::string httpVersion, int statusCode, 
		std::string contentType, std::string file);
	Header(Header const &src);
	~Header();

	Header	&operator=(Header const &rhs);

	std::string	getHeader() const;


	private:
	std::string	_httpVersion;
	int			_statusCode;
	std::string	_contentType;
	std::string	_file;
	// std::string	_contentLen;

	std::string	ft_itos(int nbr) const;

};

#endif