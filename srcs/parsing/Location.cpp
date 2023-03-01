#include "Location.hpp"

Location::Location() {}

Location::Location(int &i, std::vector<std::string> loc) : _errorLoc(false) {
	this->functPtr[0] = &Directives::setHttpMethods;
	this->functPtr[1] = &Directives::setErrorPage;
	this->functPtr[2] = &Directives::setClientMaxBodySize;
	this->functPtr[3] = &Directives::setRoot;
	this->functPtr[4] = &Directives::setAutoindex;
	this->functPtr[5] = &Directives::setIndex;
	this->functPtr[6] = &Directives::setHttpRedir;
	this->functPtr[7] = &Location::setCgi;
	this->functPtr[8] = &Location::setUpload;
	this->setPath(i, loc[1]);
}

Location::Location(Location const &src) : Directives(src) {
	*this = src;
}

Location::~Location() {}

Location	&Location::operator=(Location const &rhs) {
	Directives::operator=(rhs);

	if (this != &rhs)
	{
		this->_path = rhs._path;
		this->_errorLoc = rhs._errorLoc;
	}
	return *this;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//												G E T T E R													  //
////////////////////////////////////////////////////////////////////////////////////////////////////////////////

std::string	Location::getPath() { return this->_path; }

bool	Location::getErrorLoc() { return this->_errorLoc; }

void	Location::setPath(int &i, std::string loc) {
	for (size_t j = 0; j < loc.size(); j++)
	{
		if (!std::islower(loc[j]) and !std::isupper(loc[j]) and loc[j] != '/'
			and !isdigit(loc[j]))
			return (error_line(i, " wrong syntax of location block"));
	}
	this->_path = loc;
}

void	Location::error_line(const int &n_line, const std::string &err_msg)
{
	this->_errorLoc = true;
	std::cerr << "Error: at line " << n_line << " " << err_msg << std::endl;
}

void	Location::readLocationBlock(std::ifstream &file, int &i) {
	int j;
	std::string line;
	std::string directives[9] = { "http_methods", "error_page", "client_max_body_size",
						 "root", "autoindex", "index", "return", "cgi", "upload" };

	i += 1;
	while (std::getline(file, line))
	{
		j = 0;
		if (!only_space_or_empty(line))
		{
			std::vector<std::string> tmp = ft_split(line.c_str(), " \t");

			if (tmp.size() == 1 and tmp[0] == "}")
				return ;
			while (j < 9)
			{
				if (tmp[0] == directives[j])
				{
					if (!this->checkFormatDir(tmp, i))
					{
						this->_errorLoc = true;
						return ;
					}
					tmp[tmp.size() - 1].erase(tmp[tmp.size() - 1].size() - 1, 1);
					(this->*functPtr[j])(tmp, i);
					break ;
				}
				j++;
			}
			if (j == 9)
				error_line(i, " incorrect directive");
			if (this->_errorLoc or this->_errorDirectives)
			{
				this->_errorLoc = true;
				return ;
			}
		}
		i += 1;
	}
}
