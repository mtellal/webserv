#include "../../includes/Server.hpp"

Server::Server() : Directives(),  _host("0.0.0.0"),
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
	this->functPtr[9] = &Directives::setCgi;

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

void	Server::error_msg(const int &n_line, const std::string &err_msg)
{
	this->_errorServer = true;
	std::cerr << "Error: at line " << n_line << " " << err_msg << std::endl;
}

void	Server::setHost(std::vector<std::string> host, int *i) {
	// Comment bien verifier le host ?
	std::vector<std::string>	splitPort;
	bool						err = false;

	if (host.size() != 2)
		return (error_msg(*i, "directive listen, wrong format"));
	if (!this->checkFormatHost(host[1]))
		error_msg(*i, "directive listen, host and port must be split by one ':'");
	else
	{
		splitPort = ft_split(host[1].c_str(), ":");

		if (splitPort.size() == 2)
		{
			if (!this->checkHost(splitPort[0]))
				error_msg(*i, "directive listen, wrong syntaxe");

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
					error_msg(*i, "directive listen, wrong syntaxe");
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
		error_msg(*i, "directive listen, port must be contains only numeric values");
	else if (this->_portSet)
		error_msg(*i, "listen is already set");
	else
	{
		this->_portSet = true;
		this->_port = port;
	}
}

void	Server::setServerName(std::vector<std::string> serverName, int *i) {
	this->_serverNameSet = true;
	if (serverName.size() < 2)
		return (error_msg(*i, " directive server_name, wrong format"));
	for (size_t i = 1; i < serverName.size(); i++)
		this->_serverName.push_back(serverName[i]);
}

void	Server::readServBlock(std::ifstream &file, int *i) {
	int j;
	std::string line;
	std::string words[10] = { "listen", "server_name", "error_page", "client_max_body_size",
						 "root", "autoindex", "index", "return", "http_methods", "cgi" };

	*i += 1;
	while (std::getline(file, line))
	{
		j = 0;
		if (!only_space_or_empty(line))
		{
			std::vector<std::string> tmp = ft_split(line.c_str(), " \t");

			if (tmp.size() == 1 && tmp[0] == "}")
				return ;
			else if (this->isLocationBlock(tmp))
			{
				Location locPars(i, tmp);

				locPars.readLocationBlock(file, i);

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
				while (j < 10)
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
				}
				if (j == 10)
					error_msg(*i, "incorrect directive");
				if (this->_errorServer or this->_errorDirectives)
					return ;
			}
		}
		*i += 1;
	}
}

bool	Server::isLocationBlock(std::vector<std::string> splitLine) {
	if (splitLine.size() != 3 or splitLine[0] != "location"
		or splitLine[2] != "{")
		return false;
	for (size_t i = 0; i < splitLine[1].size(); i++)
	{
		if (!isalnum(splitLine[1][i]) and splitLine[1][i] != '/')
			return false;
	}
	return true;
}

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
		// if (tmp[i].getCgiSet())
			// o << "\t\tCgi\t\t: " << tmp[i].getCgi() << std::endl;
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