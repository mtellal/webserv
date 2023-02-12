#ifndef SERVER_HPP
# define SERVER_HPP

#include "Directives.hpp"
#include "Location.hpp"
#include <fstream>


class Server : public Directives {

	public:
	Server();
	Server(std::ifstream &file, int *i);
	Server(Server const &src);
	virtual ~Server();

	Server						&operator=(Server const &rhs);

	std::string					getHost() const;
	int							getPort() const;
	std::vector<std::string>	getServerName() const;
	std::vector<Location>		getVctLocation() const;
	bool						getErrorServer() const;
	bool						getServerNameSet() const;
	bool						getHostSet() const;
	bool						getPortSet() const;

	void						showServerName(std::ostream & o) const;
	void						showLocation(std::ostream & o, int i, Server const &rhs) const;
	void						showAutoindexBis(std::ostream & o, int i, Server const &rhs) const;
	void						showIndexBis(std::ostream & o, int i, std::vector<Location> tmp) const;


	private:
	std::vector<Location>		_vctLoation;
	std::string					_host;
	int							_port;
	std::vector<std::string>	_serverName;
	bool						_hostSet;
	bool						_portSet;
	bool						_serverNameSet;
	bool						_errorServer;
	void						(Server::*functPtr[9])(std::vector<std::string>, int *i);

	void						setHost(std::vector<std::string> host, int *i);
	bool						checkFormatHost(std::string host);
	void						setPort(std::string strPort, int *i);
	void						setServerName(std::vector<std::string> serverName, int *i);

	void						readfile(std::ifstream &file, int *i);
	bool						isLocationBlock(std::vector<std::string> splitLine);
	bool						checkHost(std::string host);

};

std::ostream &operator<<( std::ostream & o, Server const & rhs);


#endif