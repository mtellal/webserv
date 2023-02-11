#include "../../includes/Location.hpp"

Location::Location() {}

Location::Location(std::ifstream &file, int *i, std::vector<std::string> loc) : _cgiSet(false),
																		_errorLoc(false) {
	this->functPtr[0] = &Directives::setHttpMethods;
	this->functPtr[1] = &Location::setCgi;
	this->functPtr[2] = &Directives::setErrorPage;
	this->functPtr[3] = &Directives::setClientMaxBodySize;
	this->functPtr[4] = &Directives::setRoot;
	this->functPtr[5] = &Directives::setAutoindex;
	this->functPtr[6] = &Directives::setIndex;
	this->functPtr[7] = &Directives::setHttpRedir;

	this->setPath(i, loc[1]);
	this->readBlock(file, i);
}

Location::Location(Location const &src) : Directives(src) {
	*this = src;
}

Location::~Location() {}

Location	&Location::operator=(Location const &rhs) {
	Directives::operator=(rhs);

	if (this != &rhs)
	{
		// this->_httpMethods = rhs._httpMethods;
		this->_cgi = rhs._cgi;
		this->_path = rhs._path;
		// this->_httpMethodsSet = rhs._httpMethodsSet;
		this->_cgiSet = rhs._cgiSet;
		this->_errorLoc = rhs._errorLoc;
	}
	return *this;
}

// std::vector<std::string>		Location::getHttpMethods() {
// 	return this->_httpMethods;
// }

std::string						Location::getCgi() {
	return this->_cgi;
}

std::string						Location::getPath() {
	return this->_path;
}

bool	Location::getErrorLoc() {
	return this->_errorLoc;
}

bool	Location::getHttpMethodsSet() {
	return this->_httpMethodsSet;
}

bool	Location::getCgiSet() {
	return this->_cgiSet;
}

void	Location::setPath(int *i, std::string loc) {
	// chars ok : min maj / * .
	for (size_t j = 0; j < loc.size(); j++)
	{
		if (!std::islower(loc[j]) and !std::isupper(loc[j]) and !charAccepted(loc[j])
			and (loc[j] < '0' or loc[j] > '9'))
		{
			this->_errorLoc = true;
			std::cout << "Error: at line " << *i << " wrong syntax of location block" << std::endl;
			return ;
		}
	}
	this->_path = loc;
}

bool	Location::charAccepted(char c) {
	if (c == '/' or c == '*' or c == '.')
		return true;
	return false;
}

void	Location::readBlock(std::ifstream &file, int *i) {
	int j;
	std::string line;
	std::string key[8] = { "http_methods", "cgi", "error_page", "client_max_body_size",
						 "root", "autoindex", "index", "return" };

	*i += 1;
	while (getline(file, line))
	{
		j = 0;
		if (!only_space_or_empty(line))
		{
			std::vector<std::string> tmp = ft_split(line.c_str(), " \t");
			if (tmp.size() == 1 and tmp[0] == "}")
				return ;
			while (j < 8)
			{
				if (tmp[0] == key[j])
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
				if (j == 8)
				{
					this->_errorLoc = true;
					std::cout << "Error: at line " << *i << " incorrect directive" << std::endl;
				}
			}
		}
		*i += 1;
	}
}

// void	Location::setHttpMethods(std::vector<std::string> methods, int *i) {
// 	if (methods.size() < 2)
// 	{
// 		this->_errorLoc = true;
// 		std::cout << "Error: at line " << *i << " incorrect directive" << std::endl;
// 	}
// 	else
// 	{
// 		for (size_t j = 1; j < methods.size(); j++)
// 		{
// 			if (methods[j] == "GET" or methods[j] == "POST" or methods[j] == "DELETE")
// 			{
// 				this->_httpMethodsSet = true;
// 				this->_httpMethods.push_back(methods[j]);
// 			}
// 			else
// 			{
// 				this->_errorLoc = true;
// 				std::cout << "Error: at line " << *i << " incorrect HTTP methods" << std::endl;
// 			}
// 		}
// 	}
// }

void	Location::setCgi(std::vector<std::string> cgi, int *i) {
	if (cgi.size()!= 2)
	{
		this->_errorLoc = true;
		std::cout << "Error: at line " << *i << " Directive cgi, wrong args" << std::endl;
	}
	else
	{
		this->_cgiSet = true;
		this->_cgi = cgi[1];
	}
}

void	Location::showHttpMethods(std::ostream &o) {
	o << "\t\tMethods\t: ";
	for (size_t i = 0; i < this->_httpMethods.size(); i++)
	{
		o << this->_httpMethods[i];
		if (i + 1 != this->_httpMethods.size())
			o << " ";
	}
	o << std::endl;
}