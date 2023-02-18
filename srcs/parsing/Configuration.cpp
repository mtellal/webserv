#include "../../includes/Configuration.hpp"

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
		this->_vctServ = rhs._vctServ;
		this->_errorConf = rhs._errorConf;
	}
	return *this;
}

std::vector<Server>	Configuration::getVctServer() const
{
	return this->_vctServ;
}

bool	Configuration::getErrorConf()
{
	return this->_errorConf;
}

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
	std::ifstream file(path_file.c_str());
	std::string line;
	std::vector<std::string> lineSplit;
	int n_line = 1;

	if (!file)
		return (error_msg("Error: File not found"));
	while (std::getline(file, line))
	{
		lineSplit = ft_split(line.c_str(), " \t");

		if (lineSplit.size() == 2 and lineSplit[0] == "server" and lineSplit[1] == "{")
		{
			Server servPars;

			servPars.readServBlock(file, &n_line);

			if (servPars.getErrorServer() or servPars.getErrorDirectives())
			{
				error_msg();
				file.close();
				return;
			}
			else if (!servPars.getBlockClose())
			{
				error_msg("Block must be terminalet by \"}\"");
				file.close();
				return;
			}
			_vctServ.push_back(servPars);
		}
		else if (!only_space_or_empty(line))
		{
			error_msg("Error: Incorrect information at line ", n_line);
			file.close();
			return ;
		}
		n_line++;
	}
	if (this->_vctServ.empty())
		error_msg("Error: File is empty");
	file.close();
}

std::ostream &operator<<(std::ostream & o, Configuration const & rhs)
{
	std::vector<Server> tmp = rhs.getVctServer();

	if (!tmp.empty())
	{
		for (size_t i = 0; i < tmp.size(); i++)
			o << tmp[i] << std::endl;
	}
	return o;
}