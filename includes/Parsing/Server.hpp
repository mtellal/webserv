#ifndef SERVER_HPP
# define SERVER_HPP

#include <fstream>

#include "Client.hpp"
#include "Directives.hpp"
#include "Location.hpp"
#include "utils.hpp"

class Server : public Directives {

	public:

		Server();
		Server(Server const &src);
		virtual ~Server();

		Server						&operator=(Server const &rhs);

		void						setSocket(size_t fd);
		void						addClient(const Client &c);
		void						setDomain(std::string address);
		void						setAddress(std::string address);
		void						showServerName(std::ostream & o) const;
		void						readServBlock(std::ifstream &file, int &i);
		void						showCgiBis(std::ostream & o, int i, Server const &rhs) const;
		void						showLocation(std::ostream & o, int i, Server const &rhs) const;
		void						showErrorPageBis(std::ostream & o, int i, Server const &rhs) const;
		void						showAutoindexBis(std::ostream & o, int i, Server const &rhs) const;
		void						showIndexBis(std::ostream & o, int i, std::vector<Location> tmp) const;

		bool						getHostSet() const;
		bool						getPortSet() const;
		bool						getBlockClose() const;
		bool						getErrorServer() const;
		bool						getServerNameSet() const;
		bool						getListenSet() const;

		int							eraseClient(int fd);
		size_t						getFd() const;

		std::string					getHost() const;
		std::string					getPort() const;
		std::string					getDomain() const;
		std::string					getAddress() const;

		Client						getClient(int fd) const;
		std::vector<std::string>	getServerName() const;
		std::vector<Location>		getVctLocation() const;


	private:

		bool						_hostSet;
		bool						_portSet;
		bool						_blockClose;
		bool						_errorServer;
		bool						_serverNameSet;
		bool						_listenSet;

		size_t						_server_fd;
		
		std::string					_host;
		std::string					_port;
		std::string					_domain;
		std::string					_address;
		std::vector<Client>			_clients;
		std::vector<Location>		_vctLocation;
		
		std::vector<size_t>			_clients_fd;
		std::vector<std::string>	_serverName;


		void						(Server::*functPtr[11])(std::vector<std::string>, int &i);
		void						setPort(std::string strPort, int &i);
		void						setHost(std::vector<std::string> host, int &i);
		void						error_msg(const int &n_line, const std::string &err_msg);
		void						setServerName(std::vector<std::string> serverName, int &i);

		bool						checkHost(std::string host);
		bool						checkFormatHost(std::string host);
		bool						isLocationBlock(std::vector<std::string> splitLine);
};

std::ostream &operator<<( std::ostream & o, Server const & rhs);


#endif