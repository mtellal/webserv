#ifndef SERVER_HPP
# define SERVER_HPP

#include "Client.hpp"
#include "Directives.hpp"
#include "Location.hpp"
#include <fstream>

class Server : public Directives {

	public:

		Server();
		Server(Server const &src);
		virtual ~Server();

		Server						&operator=(Server const &rhs);

		std::string					getHost() const;
		std::string					getPort() const;
		std::vector<std::string>	getServerName() const;
		std::vector<Location>		getVctLocation() const;
		bool						getErrorServer() const;
		bool						getServerNameSet() const;
		bool						getHostSet() const;
		bool						getPortSet() const;
		std::string					getAddress() const;
		std::string					getDomain() const;


		void						setSocket(size_t fd);
		void						setAddress(std::string address);
		void						setDomain(std::string address);

		void						showServerName(std::ostream & o) const;
		void						showLocation(std::ostream & o, int i, Server const &rhs) const;
		void						showAutoindexBis(std::ostream & o, int i, Server const &rhs) const;
		void						showIndexBis(std::ostream & o, int i, std::vector<Location> tmp) const;
		void						readServBlock(std::ifstream &file, int *i);

		void						addClient(const Client &c);
		void						eraseClient(int fd);

	private:

		size_t						_server_fd;
		std::vector<size_t>			_clients_fd;
		std::vector<Client>			_clients;
		std::vector<Location>		_vctLocation;
		std::string					_host;
		std::string					_domain;
		std::string					_address;	// ipv4
		std::string					_port;
		std::vector<std::string>	_serverName;
		bool						_hostSet;
		bool						_portSet;
		bool						_serverNameSet;
		bool						_errorServer;
		void						(Server::*functPtr[10])(std::vector<std::string>, int *i);

		void						setHost(std::vector<std::string> host, int *i);
		bool						checkFormatHost(std::string host);
		void						setPort(std::string strPort, int *i);
		void						setServerName(std::vector<std::string> serverName, int *i);

		bool						isLocationBlock(std::vector<std::string> splitLine);
		bool						checkHost(std::string host);
		void						error_msg(const int &n_line, const std::string &err_msg);


};

std::ostream &operator<<( std::ostream & o, Server const & rhs);


#endif