#include "../includes/Request.hpp"
#include "../includes/utils.hpp"
#include <string.h>
#include <sys/epoll.h>

Request::Request() {}

Request::Request(int fd) : _fd(fd), _errRequest(false), _parsArgsGet(false),
							_closeConnection(false), _connectionSet(false), _acceptSet(false) {
	this->functPtr[0] = &Request::setMethodVersionPath;
	this->functPtr[1] = &Request::setMethodVersionPath;
	this->functPtr[2] = &Request::setMethodVersionPath;
	this->functPtr[3] = &Request::setHostPort;
	this->functPtr[4] = &Request::setConnection;
	this->functPtr[5] = &Request::setAccept;


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
		this->_parsArgsGet = rhs._parsArgsGet;
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
		this->_argsGet = rhs._argsGet;
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

bool		Request::getConnectionSet() const {
	return this->_connectionSet;
}

std::string	Request::getAccept() const {
	return this->_accept;
}

bool		Request::getAcceptSet() const {
	return this->_acceptSet;
}

// bool	Request::parsArgs(std::string tmp) {
// 	int	i = 0;
// 	int	split = 0;
// 	int	args = 0;

// 	while (tmp[i])
// 	{
// 		if (tmp[i] == '?')
// 		{
// 			if (tmp[i + 1] and tmp[i + 1] != '?' and split == 0)
// 				split++;
// 			else
// 			{
// 				// Message a modifier, renvoyer une page special ??
// 				std::cout << "1 Err arg requete" << std::endl;
// 				return false;
// 			}
// 		}
// 		else if (tmp[i] == '=')
// 		{
// 			if (!tmp[i + 1] or tmp[i + 1] == '=')
// 			{
// 				// Message a modifier, renvoyer une page special ??
// 				std::cout << "1 Err arg requete" << std::endl;
// 				return false;
// 			}
// 			this->_parsArgsGet = true;
// 			args++;
// 		}
// 		i++;
// 	}
// 	return true;
// }

void	Request::setMethodVersionPath(std::vector<std::string> strSplit) {
	std::vector<std::string>	splitBis;

	this->_method = strSplit[0];
	strSplit[2].erase(strSplit[2].size() - 1, 1);
	this->_httpVersion = strSplit[2];
	// if (!this->parsArgs(strSplit[1]))
	// 	return 1;
	strSplit = ft_split(strSplit[1].c_str(), "?");
	this->_path = strSplit[0];
	if (this->_parsArgsGet)
	{
		strSplit = ft_split(strSplit[1].c_str(), "&");
		for (size_t j = 0; j < strSplit.size(); j++)
		{
			splitBis = ft_split(strSplit[j].c_str(), "=");
			this->_argsGet.insert(std::pair<std::string, std::string>(splitBis[0], splitBis[1]));
		}
	}
}

void	Request::setHostPort(std::vector<std::string> strSplit) {
	strSplit = ft_split(strSplit[1].c_str(), ":");
	this->_host = strSplit[0];
	this->_port = strSplit[1];
}

void	Request::setConnection(std::vector<std::string> strSplit) {
	strSplit[1].erase(strSplit[1].size() - 1, 1);
	this->_connection = strSplit[1];
	this->_connectionSet = true;
}

void	Request::setAccept(std::vector<std::string> strSplit) {
	strSplit[1].erase(strSplit[1].size() - 1, 1);
	this->_accept = strSplit[1];
	this->_acceptSet = true;
}


int		Request::parsRequest(int fd) {
	char						buff[4096];
	int							oct;
	std::vector<std::string>	vct;
	std::vector<std::string>	strSplit;
	std::vector<std::string>	tmpBis;
	std::string					key[6] = { "GET", "POST", "DELETE", "Host:",
										"Connection:", "Accept:"};


	memset(buff, 0, 4096);
	oct = recv(fd, buff, 4096, 0);
	if (!oct)
	{
		this->_closeConnection = true;
		return 0;
	}
	buff[oct] = '\0';

	// std::cout << buff << std::endl;

	vct = ft_split(buff, "\n");
	for (size_t i = 0; i < vct.size(); i++)
	{
		strSplit = ft_split(vct[i].c_str(), " ");
		for (size_t j = 0; j < 6; j++)
		{
			if (strSplit[0] == key[j])
			{
				(this->*functPtr[j])(strSplit);
				break ;
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