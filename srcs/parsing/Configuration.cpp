#include "Configuration.hpp"
#include <signal.h>

Configuration::Configuration() {}

Configuration::Configuration(std::string path_file) :_errorConf(false) {
	open_and_check_file(path_file);
}

Configuration::Configuration(Configuration const &src) {
	*this = src;
}

Configuration::~Configuration() {}

Configuration &Configuration::operator=(Configuration const &rhs) {
	if (this != &rhs)
	{
		this->_servers = rhs._servers;
		this->_errorConf = rhs._errorConf;
	}
	return *this;
}

bool	Configuration::getErrorConf(){ return this->_errorConf; }

std::vector<Server>	Configuration::getVctServer() const { return this->_servers; }

void	Configuration::error_msg(const std::string &msg = "", const int &n_line = -1)
{
	if (msg.length())
	{
		std::cerr << msg;
		if (n_line != -1)
			std::cerr << n_line;
		std::cerr << std::endl;
	}
	this->_errorConf = true;
}

void	Configuration::open_and_check_file(std::string path_file) {
	std::ifstream				file(path_file.c_str());
	std::string					line;
	std::vector<std::string>	lineSplit;
	int							n_line = 1;
	bool						server_line = false;

	signal(SIGINT, SIG_IGN);
	signal(SIGQUIT, SIG_IGN);
	if (!file)
		return (error_msg("Error: File not found"));
	while (std::getline(file, line))
	{
		lineSplit = ft_split(line.c_str(), " \t");

		if ((lineSplit.size() == 2 and lineSplit[0] == "server" and lineSplit[1] == "{") ||
			(lineSplit.size() == 1 && (lineSplit[0] == "server" || lineSplit[0] == "{")))
		{
			if (server_line && lineSplit[0] != "{")
			{
				error_msg("Error: Incorrect information at line ", n_line);
				file.close();
				return ;
			}
			else if (!server_line && lineSplit.size() == 1 && lineSplit[0] == "server")
				server_line = true;
			else
			{
				Server servPars;
				server_line = false;

				servPars.readServBlock(file, n_line);

				if (servPars.getErrorServer() or servPars.getErrorDirectives())
				{
					error_msg();
					file.close();
					return;
				}
				else if (!servPars.getListenSet())
				{
					error_msg("Directive Listen must be set");
					file.close();
					return;
				}
				else if (!servPars.getBlockClose())
				{
					error_msg("Block must be terminalet by \"}\"");
					file.close();
					return;
				}
				_servers.push_back(servPars);
			}
		}
		else if (!only_space_or_empty(line))
		{
			error_msg("Error: Incorrect information at line ", n_line);
			file.close();
			return ;
		}
		n_line++;
	}
	if (this->_servers.empty())
		error_msg("Error: File is empty");
	file.close();
}

std::ostream &operator<<(std::ostream & o, Configuration const & rhs)
{
	std::vector<Server> tmp = rhs.getVctServer();

	if (!tmp.empty())
	{
		for (size_t i = 0; i < tmp.size(); i++)
		{
			o << "Server[" << i << "]" << std::endl;
			o << tmp[i] << std::endl;
		}
	}
	return o;
}