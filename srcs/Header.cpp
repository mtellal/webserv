#include "../includes/Header.hpp"
#include "../includes/utils.hpp"
#include <sstream>
#include <sys/stat.h>
#include <iostream>
#include <fstream>
#include <stdio.h>

Header::Header() {}

Header::Header(Request req, std::string file, int *statusCode) : _req(req), _statusCode(statusCode),
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
	std::string	contentType;

	contentType = this->getContentType();
	if (contentType == "406")
		*this->_statusCode = 406;

	res = this->_req.getHttpVersion() + " " + ft_itos(*this->_statusCode) + " " + this->getCodeDescription() + "\n";
	res += "Content-Type: " + this->getContentType() + "\n";
	res += "Server: webserv/1.0\n";
	res += "Date: " + this->getDate() + "\n";
	if (this->_req.getConnectionSet())
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

	std::ifstream				file("./utils/types.txt");
	std::string					line;
	std::vector<std::string>	splitLine;
	std::vector<std::string>	splitextension;
	std::string					extension;
	std::string					fileExtension;
	std::vector<std::string>	splitAccept;

	splitextension = ft_split(this->_file.c_str(), ".");
	extension = splitextension[splitextension.size() - 1];
	if (this->_req.getAcceptSet())
		splitAccept = ft_split(this->_req.getAccept().c_str(), ",;");



	while (std::getline(file, line))
	{
		splitLine = ft_split(line.c_str(), "\t ");
		for (size_t i = 1; i < splitLine.size(); i++)
		{
			if (splitLine[i] == extension)
			{
				if (this->_req.getAcceptSet())
				{
					for (size_t j = 0; j < splitAccept.size(); j++)
					{
						fileExtension = ft_split(splitLine[0].c_str(), "/")[0];
						if (splitLine[i] == splitAccept[j] or splitAccept[j] == "*/*" or
							(splitAccept[j].find(fileExtension) != std::string::npos and
							splitAccept[j].find("/*") != std::string::npos))
						{
							file.close();
							return splitLine[0];
						}
					}
				}
				else
				{
					file.close();
					return splitLine[0];
				}
			}
		}
	}
	file.close();

	if (!this->_req.getAcceptSet())
		return "text/plain";
	for (size_t i = 1; i < splitAccept.size(); i++)
	{
		if (splitAccept[i] == "*/*" or splitAccept[i] == "text/*" or
			splitAccept[i] == "text/plain")
			return "text/plain";
	}
	// Si rien ne correspond, renvoyer le code 406 (Not Acceptable)

	return "406";
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
	if (*this->_statusCode == 200)
		return "OK";
	else if (*this->_statusCode == 404)
		return "Not Found";
	else if (*this->_statusCode == 406)
		return "Not Acceptable";
	return "";
}
