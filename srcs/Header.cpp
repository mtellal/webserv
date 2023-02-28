#include "Header.hpp"
#include "utils.hpp"
#include <sstream>
#include <sys/stat.h>
#include <iostream>
#include <fstream>
#include <stdio.h>
#include <ctime>

Header::Header() {}

Header::Header(std::string file, int *statusCode, Response *rep) : _statusCode(statusCode),
				_file(file), _rep(rep){
	this->_req = this->_rep->getRequest();
	this->_serv = this->_rep->getServ();

	this->_header["Content-type"] = this->getContentType();
	this->_header["Server"] = this->_req.getServerName();
	this->_header["Date"] = this->getDate();
	this->_header["Last-Modified"] = this->getLastModified();
	this->_header["Host"] =this->_req.getHost() + ":" + this->_req.getPort();
	if (this->_req.getConnectionSet())
		this->_header["Connection"] = this->_req.getConnection();
	if (this->_req.getRefererSet())
		this->_header["Referer"] = this->_req.getReferer();
	if (this->_req.getAgentSet())
		this->_header["User-Agent"] = this->_req.getAgent();
	this->_header["Allow"] = this->getAllow();
	this->_header["Content-Length"] = this->getContentLength();
}

Header::Header(std::string file, int *statusCode) : _statusCode(statusCode), _file(file) {
	this->_header["Content-type"] = "text/html";
	this->_header["Server"] = "Webserv/1.0";
	this->_header["Date"] = this->getDate();
	this->_header["Last-Modified"] = this->getLastModified();
	this->_header["Content-Length"] = this->getContentLength();

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
		this->_serv = rhs._serv;
		this->_rep = rhs._rep;
	}
	return *this;
}

void	Header::setContentType(std::string const &contentType) {
	this->_header["Content-type"] = contentType;
}

void	Header::setContentLength(std::string const &contentLength) {
	this->_header["Content-Length"] = contentLength;
}

void	Header::setStatus(int s) {
	*this->_statusCode = s;
}

std::string	Header::getHeader() {
	std::string	res;
	std::string	contentType;

	contentType = this->getContentType();
	if (contentType == "406")
		*this->_statusCode = 406;

	res = this->_req.getHttpVersion() + " " + ft_itos(*this->_statusCode) + " " + getHttpStatusCodeMessage(*this->_statusCode) + "\n";
	res += "Content-Type: " + this->_header["Content-type"] + "\n";
	res += "Server: " + this->_header["Server"] + "\n";
	res += "Date: " + this->_header["Date"] + "\n";
	res += "Last-Modified: " + this->_header["Last-Modified"];
	res += "Host: " + this->_header["Host"] + "\n";
	if (this->_req.getConnectionSet())
		res += "Connection: " + this->_header["Connection"] + "\n";
	if (this->_req.getRefererSet())
		res += "Referer: " + this->_header["Referer"] + "\n";
	if (this->_req.getAgentSet())
		res += "User-Agent: " + this->_header["User-Agent"] + "\n";
	res += "Allow: " + this->_header["Allow"] + "\n";
	res += "Content-Length: " + this->_header["Content-Length"] + "\n\n";

	return res;
}

std::string	Header::getHeaderRequestError() {
	std::string	res;

	res = "HTTP/1.1 " + ft_itos(*this->_statusCode) + " " + getHttpStatusCodeMessage(*this->_statusCode) + "\n";
	res += "Content-Type: text/html\n";
	res += "Server: " + this->_header["Server"] + "\n";
	res += "Date: " + this->_header["Date"] + "\n";
	res += "Last-Modified: " + this->_header["Last-Modified"];
	res += "Content-Length: " + this->_header["Content-Length"] + "\n\n";

	return res;
}

std::string	Header::ft_itos(int nbr) const {
	std::string s;
	std::stringstream out;

	out << nbr;
	s = out.str();
	return s;
}

std::string	Header::getContentType() {

	std::string					type;
	std::string					extension;
	std::vector<std::string>	splitAccept;

	if (this->_req.getAcceptSet())
		splitAccept = ft_split(this->_req.getAccept().c_str(), ",;");
	type = this->parsContentTypeFile(splitAccept);
	if (type != "")
		return type;

	if (!this->_req.getAcceptSet())
		return "text/plain";
	for (size_t i = 1; i < splitAccept.size(); i++)
	{
		if (splitAccept[i] == "*/*" or splitAccept[i] == "text/*" or
			splitAccept[i] == "text/plain")
			return "text/plain";
	}
	return "406";
}

std::string	Header::parsContentTypeFile(std::vector<std::string> splitAccept) const {
	std::vector<std::string>	splitLine;
	std::ifstream				file("./utils/types.txt");
	std::string					extension;
	std::string					fileExtension;
	std::vector<std::string>	splitextension;
	std::string					line;


	splitextension = ft_split(this->_file.c_str(), ".");
	extension = splitextension[splitextension.size() - 1];
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
	return "";
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

std::string	Header::getLastModified() const {
	struct stat s;

	stat(this->_file.c_str(), &s);
	return ctime(&s.st_mtime);
}

std::string	Header::getAllow() const {
	std::string					ret;
	std::vector<std::string>	allow;

	if (this->_rep->getlocBlocSelect() and this->_rep->getLocBloc().getHttpMethodsSet())
		allow = this->_rep->getLocBloc().getHttpMethods();
	else
		allow = this->_rep->getServ().getHttpMethods();

	for (size_t i = 0; i < allow.size(); i++)
	{
		if (i != 0)
			ret += ", ";
		ret += allow[i];
	}
	return ret;
}
