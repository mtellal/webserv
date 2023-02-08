#include "../includes/Header.hpp"
#include <sstream>
#include <sys/stat.h>
#include <iostream>

Header::Header() {}

Header::Header(Request req, std::string file, int statusCode) : _req(req), _statusCode(statusCode),
				_file(file) {
}

Header::Header(Header const &src) {
	*this = src;
}

Header::~Header() {}

Header	&Header::operator=(Header const &rhs) {
	if (this != &rhs)
	{
		this->_req = rhs._req;
		this->_statusCode = rhs._statusCode;
		this->_file = rhs._file;
	}
	return *this;
}

std::string	Header::getHeader() const {
	std::string	res;

	res = this->_req.getHttpVersion() + " " + ft_itos(this->_statusCode) + " " + this->getCodeDescription() + "\n";
	res += "Content-Type: " + this->getContentType() + "\n";
	res += "Server: webserv/1.0\n";
	res += "Date: " + this->getDate() + "\n";
	res += "Connection: " + this->_req.getConnection() + "\n";
	res += "Content-Length: " + this->getContentLength() + "\n\n";

	// std::cout << res << std::endl;

	return res;
}

std::string	Header::ft_itos(int nbr) const {
	std::string s;
	std::stringstream out;

	out << nbr;
	s = out.str();
	return s;
}

std::string	Header::getContentType() const {
	return "text/html";
}


std::string	Header::getDate() const {
	std::string date;
	time_t tmm = time(0);

	tm *g = gmtime(&tmm);
	date = asctime(g);
	date.erase(date.size() - 1, 1);
	date += " GMT";

	return date;
}

std::string	Header::getContentLength() const {
	struct stat	stats;

	stat(this->_file.c_str(), &stats);
	return ft_itos(stats.st_size);
}

std::string	Header::getCodeDescription() const {
	if (this->_statusCode == 200)
		return "OK";
	else if (this->_statusCode == 404)
		return "Not Found";
	return "";
}
