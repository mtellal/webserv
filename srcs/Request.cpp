#include "../includes/Request.hpp"
#include "../includes/utils.hpp"
#include <string.h>
#include <sys/epoll.h>

Request::Request() {}

Request::Request(int fd) : _fd(fd), _errRequest(false), _parsArgsGet(false),
							_closeConnection(false){
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
		this->_method = rhs._method;
		this->_path = rhs._path;
		this->_httpVersion = rhs._httpVersion;
		this->_host = rhs._host;
		this->_port = rhs._port;
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

int		Request::parsRequest(int fd) {
	char						buff[4096];
	std::vector<std::string>	vct;
	std::vector<std::string>	tmp;
	std::vector<std::string>	tmpBis;

	memset(buff, 0, 4096);
	int t = recv(fd, buff, 4096, 0);
	if (!t)
	{
		this->_closeConnection = true;
		return 0;
	}
	buff[t] = '\0';

	// std::cout << buff << std::endl;

	vct = ft_split(buff, "\n");
	// for (size_t i = 0; i < vct.size(); i++)
		// std::cout << vct[i] << std::endl;

	for (size_t i = 0; i < vct.size(); i++)
	{
		tmp = ft_split(vct[i].c_str(), " ");
		if (tmp[0] == "GET" or tmp[0] == "POST" or tmp[0] == "DELETE")
		{
			this->_method = tmp[0];
			this->_httpVersion = tmp[2];
			// if (!this->parsArgs(tmp[1]))
			// 	return 1;
			tmp = ft_split(tmp[1].c_str(), "?");
			this->_path = tmp[0];
			if (this->_parsArgsGet)
			{
				tmp = ft_split(tmp[1].c_str(), "&");
				for (size_t j = 0; j < tmp.size(); j++)
				{
					tmpBis = ft_split(tmp[j].c_str(), "=");
					this->_argsGet.insert(std::pair<std::string, std::string>(tmpBis[0], tmpBis[1]));
				}
			}
		}
		else if (tmp[0] == "Host:")
		{
			tmp = ft_split(tmp[1].c_str(), ":");
			this->_host = tmp[0];
			this->_port = tmp[1];
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