#include "../../includes/Configuration.hpp"

Configuration::Configuration() {}

Configuration::Configuration(std::string path_file) : _errorConf(false) {
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

std::vector<Server>	Configuration::getVctServer() const {
	return this->_vctServ;
}

bool	Configuration::get_errorConf() {
	return this->_errorConf;
}

void	Configuration::open_and_check_file(std::string path_file) {
	std::ifstream file(path_file.c_str());
	std::string line;
	std::vector<std::string> lineSplit;
	int i = 1;

	if (!file)
	{
		this->_errorConf = true;
		std::cout << "Error: File not found" << std::endl;
		return ;
	}
	while (std::getline(file, line))
	{
		lineSplit = ft_split(line.c_str(), " \t");
		if (lineSplit.size() == 2 and lineSplit[0] == "server" and lineSplit[1] == "{")
		{
			Server servPars(file, &i);
			if (servPars.getErrorServer() or servPars.getErrorDirectives())
			{
				this->_errorConf = true;
				file.close();
				return;
			}
			_vctServ.push_back(servPars);
		}
		else if (!only_space_or_empty(line))
		{
			this->_errorConf = true;
			std::cout << "Error: Incorrect information at line " << i << std::endl;
			file.close();
			return ;
		}
		i++;
	}
	if (this->_vctServ.empty())
	{
		this->_errorConf = true;
		std::cout << "Error: File is empty" << std::endl;
	}
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