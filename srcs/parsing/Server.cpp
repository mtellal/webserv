#include "../../includes/Server.hpp"

Server::Server() {}

Server::Server(std::ifstream &file, int *i) : Directives(),  _host("0.0.0.0"),
									_port(8080), _hostSet(false), _portSet(false), 
									_serverNameSet(false), _errorServer(false) {
	this->functPtr[0] = &Server::setHost;
	this->functPtr[1] = &Server::setServerName;
	this->functPtr[2] = &Directives::setErrorPage;
	this->functPtr[3] = &Directives::setClientMaxBodySize;
	this->functPtr[4] = &Directives::setRoot;
	this->functPtr[5] = &Directives::setAutoindex;
	this->functPtr[6] = &Directives::setIndex;
	this->functPtr[7] = &Directives::setHttpRedir;
	this->functPtr[8] = &Directives::setHttpMethods;

	this->readfile(file, i);
}

Server::Server(Server const &src) : Directives(src) {
	*this = src;
}

Server::~Server() {}

Server	&Server::operator=(Server const &rhs) {
	Directives::operator=(rhs);

	if (this != &rhs)
	{
		this->_vctLoation = rhs._vctLoation;
		this->_host = rhs._host;
		this->_port = rhs._port;
		this->_serverName = rhs._serverName;
		this->_hostSet = rhs._hostSet;
		this->_portSet = rhs._portSet;
		this->_serverNameSet = rhs._serverNameSet;
		this->_errorServer = rhs._errorServer;
	}
	return *this;
}

std::string	Server::getHost() const {
	return this->_host;
}

int			Server::getPort() const {
	return this->_port;
}

std::vector<std::string>	Server::getServerName() const {
	return this->_serverName;
}

std::vector<Location>		Server::getVctLocation() const {
	return this->_vctLoation;
}

bool	Server::getErrorServer() const {
	return this->_errorServer;
}

bool	Server::getServerNameSet() const {
	return this->_serverNameSet;
}

bool	Server::getHostSet() const {
	return this->_hostSet;
}

bool	Server::getPortSet() const {
	return this->_portSet;
}

bool	Server::checkFormatHost(std::string host) {
	int i = 0;

	for (int j = 0; host[j]; j++)
	{
		if (host[j] == ':')
			i++;
	}
	if (i > 1)
		return false;
	return true;

}


void	Server::setHost(std::vector<std::string> host, int *i) {
	// Comment bien verifier le host ?
	std::vector<std::string>	splitPort;
	bool						err = false;

	if (host.size() != 2)
	{
		this->_errorServer = true;
		std::cout << "Error: at line " << *i << " directive listen, wrong format" << std::endl;
		return ;
	}
	if (!this->checkFormatHost(host[1]))
	{
		this->_errorServer = true;
		std::cout << "Error: at line " << *i << " directive listen, host and port must be split by one ':'" << std::endl;
	}
	else
	{
		splitPort = ft_split(host[1].c_str(), ":");

		if (splitPort.size() == 2)
		{
			if (!this->checkHost(splitPort[0]))
			{
				this->_errorServer = true;
				std::cout << "Error: at line " << *i << " directive listen, wrong syntaxe" << std::endl;
				return ;
			}
			this->_hostSet = true;
			this->_host = splitPort[0];
			this->setPort(splitPort[1], i);
		}
		else
		{
			if (checkHost(host[1]))
			{
				this->_hostSet = true;
				this->_host = host[1];
				// this->setPort(this->_port, i);
			}
			else
			{
				ft_stoi(host[1], &err);
				if (err)
				{
					this->_errorServer = true;
					std::cout << "Error: at line " << *i << " directive listen, wrong syntaxe" << std::endl;
				}
				else
				{
					this->_hostSet = true;
					this->_host = "0.0.0.0";
					this->setPort(host[1], i);
				}
			}
		}
	}
}

bool	Server::checkHost(std::string host) {
	std::vector<std::string> splitHost;
	bool	err;
	int		nb;

	if (host == "localhost")
		host = "127.0.0.1";
	splitHost = ft_split(host.c_str(), ".");
	if (splitHost.size() != 4)
		return false;
	for (int j = 0; j < 4; j++)
	{
		err = false;
		nb = ft_stoi(splitHost[j], &err);
		if (err or nb < 0 or nb > 255)
			return false;
	}
	return true;
}


void	Server::setPort(std::string strPort, int *i) {
	// Verifier le port ?
	bool err = false;
	int port = ft_stoi(strPort, &err);

	if (err)
	{
		this->_errorServer = true;
		std::cout << "Error: at line " << *i << " directive listen, port must be contains only numeric values" << std::endl;
	}
	else if (this->_portSet)
	{
		this->_errorServer = true;
		std::cout << "Error: at line " << *i << " listen is already set" << std::endl;
	}
	else
	{
		this->_portSet = true;
		this->_port = port;
	}
}

void	Server::setServerName(std::vector<std::string> serverName, int *i) {
	this->_serverNameSet = true;
	if (serverName.size() < 2)
	{
		this->_errorServer = true;
		std::cout << "Error: at line " << *i << " directive server_name, wrong format" << std::endl;

	}
	for (size_t i = 1; i < serverName.size(); i++)
		this->_serverName.push_back(serverName[i]);
}

void	Server::readfile(std::ifstream &file, int *i) {
	int j;
	std::string line;
	std::string words[9] = { "listen", "server_name", "error_page", "client_max_body_size",
						 "root", "autoindex", "index", "return", "http_methods" };

	*i += 1;
	while (std::getline(file, line))
	{
		j = 0;
		if (!only_space_or_empty(line))
		{
			std::vector<std::string> tmp = ft_split(line.c_str(), " \t");
			if (tmp.size() == 1 and tmp[0] == "}")
				return ;
			else if (this->isLocationBlock(tmp))
			{
				Location locPars(file, i, tmp);
				// Verifier aussi que le block n'est pas vide ??
				if (locPars.getErrorLoc())
				{
					this->_errorServer = true;
					return ;
				}
				this->_vctLoation.push_back(locPars);
			}
			else
			{
				while (j < 9)
				{
					if (tmp[0] == words[j])
					{
						if (!this->checkFormatDir(tmp, i))
						{
							this->_errorServer = true;
							return ;
						}
						tmp[tmp.size() - 1].erase(tmp[tmp.size() - 1].size() - 1, 1);
						(this->*functPtr[j])(tmp, i);
						break ;
					}
					j++;
					if (j == 9)
					{
						this->_errorServer = true;
						std::cout << "Error: at line " << *i << " incorrect directive" << std::endl;
					}
				}
			}
		}
		*i += 1;
	}
}

bool	Server::isLocationBlock(std::vector<std::string> splitLine) {
	if (splitLine.size() == 3 and splitLine[0] == "location"
		and splitLine[2] == "{")
		return true;
	return false;
}

// bool	Server::checkFormatDir(std::vector<std::string> host, int *i) {
// 	std::string	tmp = host[host.size() - 1];
// 	int			len = tmp.size();

// 	if (tmp[len - 1] != ';')
// 	{
// 		this->_errorServer = true;
// 		std::cout << "Error: at line " << *i << " directive must be terminated by \';\'" << std::endl;
// 		return false;
// 	}
// 	std::cout << host[host.size() - 1] << std::endl;
// 	host[host.size() - 1].erase(len - 1, 1);
// 	std::cout << host[host.size() - 1] << std::endl;
// 	return true;
// }


void	Server::showServerName(std::ostream & o) const {
	if (this->_serverNameSet)
	{
		o << "Server Name\t:";
		for (size_t i = 0; i < this->_serverName.size(); i++)
			o << " " << this->_serverName[i];
	}
	o << std::endl;
}

void	Server::showLocation(std::ostream & o, int i, Server const &rhs) const {
		std::vector<Location> tmp = rhs.getVctLocation();

		o << std::endl;
		o << "\t\tLocation: " << tmp[i].getPath() << std::endl;
		if (tmp[i].getHttpMethodsSet())
			tmp[i].showHttpMethods(o);
		if (tmp[i].getCgiSet())
			o << "\t\tCgi\t\t: " << tmp[i].getCgi() << std::endl;
		// if (tmp[i].getErrorPageSet())
		// 	rhs.showErrorPageBis(o, i, rhs);
		if (tmp[i].getClientMaxBodySizeSet())
			o << "\t\tclient max\t: " << rhs._vctLoation[i].getClientMaxBodySize() << std::endl;
		if (tmp[i].getRootSet())
			o << "\t\tRoot\t: " << rhs._vctLoation[i].getRoot() << std::endl;
		if (tmp[i].getAutoindexSet())
			rhs.showAutoindexBis(o, i, rhs);
		if (tmp[i].getIndexSet())
			rhs.showIndexBis(o, i, tmp);
		if (tmp[i].getHttpRedirSet())
			o << "\t\tHttp redir\t: " << tmp[i].getHttpRedir() << std::endl;
		o << std::endl;
}

void	Server::showIndexBis(std::ostream & o, int i, std::vector<Location> tmp) const {
			o << "\t\tIndex\t:";
			for (size_t j = 0; j < tmp[i].getIndex().size(); j++)
				o << " " << tmp[i].getIndex()[j];
			o << std::endl;
}

void	Server::showAutoindexBis(std::ostream & o, int i, Server const &rhs) const {
	o << "\t\tAutoindex\t: ";
	if (rhs.getVctLocation()[i].getAutoindex())
		o << "on";
	else
		o << "off";
	o << std::endl;
}

// void	Server::showErrorPageBis(std::ostream & o, int i, Server const &rhs) const {
// 	o << "\t\tError Page\t:";
// 	std::vector<Location> tmp = rhs.getVctLocation();
// 		for (size_t j = 0; j < tmp[i].getErrorPage().size(); j++)
// 			o << " (" << tmp[i].getErrorPage()[j].first << " " << tmp[i].getErrorPage()[j].second << ")";
// 	o << std::endl;
// }

std::ostream &operator<<(std::ostream & o, Server const &rhs)
{
	if (rhs.getServerNameSet())
		o << "Server:" << std::endl;
	if (rhs.getHostSet())
		o << "Host\t\t: " << rhs.getHost() << std::endl;
	if (rhs.getPortSet())
		o << "Port\t\t: " << rhs.getPort() << std::endl;
	rhs.showServerName(o);
	rhs.showErrorPage(o);
	if (rhs.getClientMaxBodySizeSet())
		o << "client max\t: " << rhs.getClientMaxBodySize() << std::endl;
	if (rhs.getRootSet())
		o << "Root\t\t: " << rhs.getRoot() << std::endl;
	rhs.showAutoindex(o);
	rhs.showIndex(o);
	if (rhs.getHttpRedirSet())
		o << "Http redir\t: " << rhs.getHttpRedir() << std::endl;
	for (size_t i = 0; i < rhs.getVctLocation().size(); i++)
		rhs.showLocation(o, i, rhs);
	return o;
}