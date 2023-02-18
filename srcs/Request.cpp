#include "../includes/Request.hpp"
#include "../includes/utils.hpp"
#include <string.h>
#include <sys/epoll.h>

Request::Request() {}

Request::Request(int fd) : _fd(fd), _errRequest(false), _queryStringSet(false), _boundarySet(false),
							_closeConnection(false), _connectionSet(false), _acceptSet(false),
							_refererSet(false), _agentSet(false), _serverName("Webserv/1.0") {
	this->functPtr[0] = &Request::setMethodVersionPath;
	this->functPtr[1] = &Request::setMethodVersionPath;
	this->functPtr[2] = &Request::setMethodVersionPath;
	this->functPtr[3] = &Request::setMethodVersionPath;
	this->functPtr[4] = &Request::setHostPort;
	this->functPtr[5] = &Request::setConnection;
	this->functPtr[6] = &Request::setAccept;
	this->functPtr[7] = &Request::setReferer;
	this->functPtr[8] = &Request::setAgent;
	this->functPtr[9] = &Request::setAuthentification;
	this->functPtr[10] = &Request::setContentLength;
	this->functPtr[11] = &Request::setContentType;

	if (parsRequest(fd))
		this->_errRequest = true;
}

Request::Request(Request const &src) {
	*this = src;
}

Request::~Request() {}

Request	&Request::operator=(Request const &rhs) {
	if (this != &rhs)
	{
		this->_fd = rhs._fd;
		this->_errRequest = rhs._errRequest;
		this->_queryStringSet = rhs._queryStringSet;
		this->_boundarySet = rhs._boundarySet;
		this->_closeConnection = rhs._closeConnection;
		this->_method = rhs._method;
		this->_path = rhs._path;
		this->_httpVersion = rhs._httpVersion;
		this->_host = rhs._host;
		this->_port = rhs._port;
		this->_connection = rhs._connection;
		this->_connectionSet = rhs._connectionSet;
		this->_accept = rhs._accept;
		this->_acceptSet = rhs._acceptSet;
		this->_queryString = rhs._queryString;
		this->_refererSet = rhs._refererSet;
		this->_referer = rhs._referer;
		this->_agentSet = rhs._agentSet;
		this->_agent = rhs._agent;
		this->_serverName = rhs._serverName;
	}
	return *this;
}

int			Request::getFd() const {
	return this->_fd;
}

bool		Request::getErrRequest() const {
	return this->_errRequest;
}

bool		Request::getcloseConnection() const {
	return this->_closeConnection;
}

std::string	Request::getMethod() const {
	return this->_method;
}

std::string	Request::getPath() const {
	return this->_path;
}

std::string	Request::getHttpVersion() const {
	return this->_httpVersion;
}

std::string	Request::getHost() const {
	return this->_host;
}

std::string	Request::getPort() const {
	return this->_port;
}

std::string	Request::getConnection() const {
	return this->_connection;
}

std::string	Request::getReferer() const {
	return this->_referer;
}

std::string	Request::getAgent() const {
	return this->_agent;
}

std::string	Request::getServerName() const {
	return this->_serverName;
}

std::string	Request::getAuthentification() const {
	return this->_authentification;
}
std::string	Request::getContentLength() const {
	return this->_contentLength;
}

std::string	Request::getContentType() const {
	return this->_contentType;
}

std::string	Request::getAccept() const {
	return this->_accept;
}

std::map<std::string, std::string>	Request::getQueryString() const {
	return this->_queryString;
}

bool		Request::getConnectionSet() const {
	return this->_connectionSet;
}

bool		Request::getAcceptSet() const {
	return this->_acceptSet;
}

bool		Request::getRefererSet() const {
	return this->_refererSet;
}

bool		Request::getAgentSet() const {
	return this->_agentSet;
}

void	Request::parsArgs(std::string arg) {
	std::vector<std::string>	args;
	std::vector<std::string>	keyValue;

	this->_queryStringSet = true;
	args = ft_split(arg.c_str(), "&");
	for (size_t i = 0; i < args.size(); i++)
	{
		keyValue = ft_split(args[i].c_str(), "=");
		if (keyValue.size() == 1)
			this->_queryString.insert(std::make_pair(keyValue[0], ""));
		else
			this->_queryString.insert(std::make_pair(keyValue[0], keyValue[1]));
	}
}

void	Request::setMethodVersionPath(std::vector<std::string> strSplit) {
	std::vector<std::string>	splitBis;

	this->_method = strSplit[0];
	this->_httpVersion = strSplit[2];
	strSplit = ft_split(strSplit[1].c_str(), "?");
	if (strSplit.size() == 2)
		this->parsArgs(strSplit[1]);
	this->_path = strSplit[0];
}

void	Request::setHostPort(std::vector<std::string> strSplit) {
	strSplit = ft_split(strSplit[1].c_str(), ":");
	this->_host = strSplit[0];
	this->_port = strSplit[1];
}

void	Request::setConnection(std::vector<std::string> strSplit) {
	this->_connection = strSplit[1];
	this->_connectionSet = true;
}

void	Request::setAccept(std::vector<std::string> strSplit) {
	this->_accept = strSplit[1];
	this->_acceptSet = true;
}

void	Request::setReferer(std::vector<std::string> strSplit) {
	this->_referer = strSplit[1];
	this->_refererSet = true;
}

void	Request::setAgent(std::vector<std::string> strSplit) {
	for (size_t i = 1; i < strSplit.size(); i++)
	{
		if (i != 1)
			this->_agent += " ";
		this->_agent += strSplit[i];
	}
	this->_agentSet = true;
}

void	Request::setAuthentification(std::vector<std::string> strSplit) {
	for (size_t i = 1; i < strSplit.size(); i++)
	{
		if (i != 1)
			this->_authentification += " ";
		this->_authentification += strSplit[i];
	}
}

void	Request::setContentLength(std::vector<std::string> strSplit) {
	this->_contentLength = strSplit[1];
}

void	Request::setContentType(std::vector<std::string> strSplit) {
	for (size_t i = 1; i < strSplit.size(); i++)
	{
		if (i != 1)
			this->_contentType += " ";
		this->_contentType += strSplit[i];
	}
	if (strSplit.size() == 3 and strSplit[1] == "multipart/form-data;" and
		strSplit[2].find("boundary=") != std::string::npos)
	{
		strSplit = ft_split(strSplit[2].c_str(), "=\"");
		this->_boundarySet = true;
		this->_boundary = "--";
		this->_boundary += strSplit[1];
	}
}

void	Request::setGetParams(std::vector<std::string> vct, size_t *i) {
	std::vector<std::string>	strSplit;
	std::string					endBoundary = this->_boundary + "--";
	std::string					name;
	std::string					value;

	while (*i < vct.size() and vct[*i] != endBoundary)
	{
		strSplit = ft_split(vct[*i].c_str(), " ");
		if (strSplit[0] == "Content-Disposition:")
		{
			for (size_t j = 0; j < strSplit.size(); j++)
			{
				if (strSplit[j].find("name=") != std::string::npos)
				{
					// Tester si dans le code html, il n'y a pas de name ou que le name est vide
					name = ft_split(strSplit[j].c_str(), "=\"")[1];
					break ;
				}
			}
			*i += 2;
			value = vct[*i];
			this->_queryString.insert(std::make_pair(name, value));
		}
		*i += 1;
	}
}

int		Request::parsRequest(int fd)
{
	size_t						bufflen = 4096;
	char						buff[bufflen];
	int							oct;
	std::vector<std::string>	vct;
	std::vector<std::string>	strSplit;
	std::vector<std::string>	strSplitBoundary;
	std::vector<std::string>	tmpBis;
	std::string					key[12] = { "GET", "HEAD", "POST", "DELETE", "Host:",
					"Connection:", "Accept:", "Referer:", "User-Agent:", "Authentification:",
					"Content-Length:", "Content-Type:"};

	std::string request;

	while ((oct = recv(fd, buff, bufflen - 1, 0)) > 0)
	{
		buff[oct] = '\0';
		request.append(buff);
		if (oct < (int)bufflen - 1)
			break ;
	}

	if (oct < 1)
	{
		if (!oct)
		{
			this->_closeConnection = true;
			return (0);
		}
		else if (oct == -1)
		{
			std::cout << "recv call failed" << std::endl;
			return (1);
		}
	}

	vct = ft_split(request, "\n\r");
	for (size_t i = 0; i < vct.size(); i++)
	// 	std::cout << vct[i] << std::endl;
	// std::cout << std::endl;

	for (size_t i = 0; i < vct.size(); i++)
	{
		strSplit = ft_split(vct[i].c_str(), " ");
		if (this->_boundarySet and strSplit[0] == this->_boundary)
			this->setGetParams(vct, &i);
		else
		{
			for (size_t j = 0; j < 12; j++)
			{
				if (strSplit[0] == key[j])
				{
					(this->*functPtr[j])(strSplit);
					break ;
				}
			}
		}
	}
	return 0;
}

std::ostream &operator<<(std::ostream & o, Request const & rhs) {
	o << "method = " << rhs.getMethod() << std::endl;
	o << "path = " << rhs.getPath() << std::endl;
	o << "httpVersion = " << rhs.getHttpVersion() << std::endl;
	o << "host = " << rhs.getHost() << std::endl;
	o << "port = " << rhs.getPort() << std::endl;
	return o;
}