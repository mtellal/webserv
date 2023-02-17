#ifndef CONFIGURATION_HPP
# define CONFIGURATION_HPP

#include "Server.hpp"

class Configuration {

	public:

		Configuration();
		Configuration(std::string path_file);
		Configuration(Configuration const &src);
		virtual ~Configuration();

		Configuration &operator=(Configuration const &rhs);

		std::vector<Server> getVctServer() const;
		bool				get_errorConf();


	private:

		std::vector<Server> _servers;
		bool				_errorConf;

		void				open_and_check_file(std::string path_file);
		void				error_msg(const std::string &msg, const int &n_line);
};

std::ostream &operator<<( std::ostream & o, Configuration const & rhs);

#endif