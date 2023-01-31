#include "../includes/Header.hpp"
#include <sstream>
#include <sys/stat.h>

Header::Header() {}

Header::Header(std::string httpVersion, int statusCode, 
	std::string contentType, std::string file) : 
	_httpVersion(httpVersion), _statusCode(statusCode),
	_contentType(contentType), _file(file) {
}

Header::Header(Header const &src) {
	*this = src;
}

Header::~Header() {}

Header	&Header::operator=(Header const &rhs) {
	if (this != &rhs)
	{
		this->_httpVersion = rhs._httpVersion;
		this->_statusCode = rhs._statusCode;
		this->_contentType = rhs._contentType;
		// this->_contentLen = rhs._contentLen;
	}
	return *this;
}

std::string	Header::getHeader() const {
	std::string res;

	// std::string s;
	// std::stringstream out;
	// out << this->_statusCode;
	// s = out.str();

	struct stat stats;
	stat(this->_file.c_str(), &stats);

	res = this->_httpVersion + " " + ft_itos(this->_statusCode) + " " + "OK" +
	"\nContent-Type: text/html\nContent-Length: " + ft_itos(stats.st_size) + "\n\n";
	return res;
}

std::string	Header::ft_itos(int nbr) const {
	std::string s;
	std::stringstream out;

	out << nbr;
	s = out.str();
	return s;
}
