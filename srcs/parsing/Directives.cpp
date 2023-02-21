#include "Directives.hpp"
#include "utils.hpp"


Directives::Directives() :  _clientMaxBodySize(1), _root("html"), _autoindex(false),
						_errorPageSet(false), _clientMaxBodySizeSet(false),
						_rootSet(false), _autoindexSet(false), _indexSet(false),
						_httpRedirSet(false), _errorDirectives(false), _httpMethodsSet(false) {
	this->_index.push_back("index.html");
	this->_httpMethods.push_back("GET");
}

Directives::Directives(Directives const &src) {
	*this = src;
}

Directives::~Directives() {}

Directives	&Directives::operator=(Directives const &rhs) {
	if (this != &rhs)
	{
		this->_errorPage = rhs._errorPage;
		this->_clientMaxBodySize = rhs._clientMaxBodySize;
		this->_root = rhs._root;
		this->_autoindex = rhs._autoindex;
		this->_index = rhs._index;
		this->_httpRedir = rhs._httpRedir;
		this->_errorPageSet = rhs._errorPageSet;
		this->_clientMaxBodySizeSet = rhs._clientMaxBodySizeSet;
		this->_rootSet = rhs._rootSet;
		this->_autoindexSet = rhs._autoindexSet;
		this->_indexSet = rhs._indexSet;
		this->_httpRedirSet = rhs._httpRedirSet;
		this->_errorDirectives = rhs._errorDirectives;
		this->_httpMethods = rhs._httpMethods;
		this->_httpMethodsSet = rhs._httpMethodsSet;
		this->_cgi = rhs._cgi;
	}
	return *this;
}

std::map<int, std::string>	Directives::getErrorPage() const {
	return this->_errorPage;
}

int							Directives::getClientMaxBodySize() const {
	return this->_clientMaxBodySize;
}

std::string					Directives::getRoot() const {
	return this->_root;
}

bool						Directives::getAutoindex() const {
	return this->_autoindex;
}

std::vector<std::string>	Directives::getIndex() const {
	return this->_index;
}

std::string					Directives::getHttpRedir() const {
	return this->_httpRedir;
}

bool						Directives::getErrorDirectives() const {
	return this->_errorDirectives;
}

bool						Directives::getErrorPageSet() const {
	return this->_errorPageSet;
}

bool						Directives::getClientMaxBodySizeSet() const {
	return this->_clientMaxBodySizeSet;
}

bool						Directives::getRootSet() const {
	return this->_rootSet;
}

bool						Directives::getAutoindexSet() const {
	return this->_autoindexSet;
}

bool						Directives::getIndexSet() const {
	return this->_indexSet;
}

bool						Directives::getHttpRedirSet() const {
	return this->_httpRedirSet;
}

std::vector<std::string>	Directives::getHttpMethods() const {
	return this->_httpMethods;
}

std::map<std::string, std::string>	Directives::getCgi() const {
	return this->_cgi;
}

void	Directives::error_line(const int &n_line, const std::string &err_msg)
{
	this->_errorDirectives = true;
	std::cerr << "Error: at line " << n_line << " " << err_msg << std::endl;
}

void	Directives::setErrorPage(std::vector<std::string> str, int *i) {
	bool	err = false;
	int		nbError;

	if (str.size() != 3)
		return (error_line(*i, "directive error_page, wrong format"));
	nbError = ft_stoi(str[1], &err);
	if (err)
		error_line(*i, "code error_page is incorrect");
	else if ((nbError >= 400 and nbError <= 418) or (nbError >= 421 and nbError <= 426)
		or (nbError >= 428 and nbError <= 429) or nbError == 431 or (nbError >= 449 and nbError <= 451)
		or nbError == 456)
	{
		if (!this->_errorPageSet)
			this->_errorPageSet = true;
		this->_errorPage[nbError] = str[2];
	}
	else
		error_line(*i, "code error_page don't exist");
}

void	Directives::setClientMaxBodySize(std::vector<std::string> maxClient, int *i) {
	bool	err = false;
	int		nbMaxClient;

	if (maxClient.size() != 2)
		return (error_line(*i, "directive client_max_body_size, wrong format"));
	nbMaxClient= ft_stoi(maxClient[1], &err);
	if (err)
		error_line(*i, "client_max_body_size must be contains only numeric values");
	if (nbMaxClient >= 0)
	{
		if (!this->_clientMaxBodySizeSet)
		{
			this->_clientMaxBodySizeSet = true;
			this->_clientMaxBodySize = nbMaxClient;
		}
		else
			error_line(*i, "client_max_body_size is already set");
	}
	else
		error_line(*i, "client_max_body_size must be positive");
}

void	Directives::setRoot(std::vector<std::string> root, int *i) {
	if (root.size() != 2)
		error_line(*i, "directive root, wrong format");
	else if (!this->_rootSet)
	{
		this->_rootSet = true;
		this->_root = root[1];
	}
	else
		error_line(*i, "root is already set");
}

void	Directives::setAutoindex(std::vector<std::string> autoindex, int *i) {
	if (autoindex.size() != 2)
		error_line(*i, "directive autoindex, wrong format");
	else if (!this->_autoindexSet and (autoindex[1] == "on" or autoindex[1] == "off"))
	{
			this->_autoindexSet = true;
		if (autoindex[1] == "on")
			this->_autoindex = true;
		else
			this->_autoindex = false;
	}
	else if (!this->_autoindexSet)
		error_line(*i, "directive autoindex, value must be on or off");
	else
		error_line(*i, "autoindex is already set");
}

void	Directives::setIndex(std::vector<std::string> index, int *i) {
	if (index.size() < 2)
		error_line(*i, "directive index wrong format");
	else if (!this->_indexSet)
	{
		this->_index.clear();
		this->_indexSet = true;
		for (size_t i = 1; i < index.size(); i++)
			this->_index.push_back(index[i]);
	}
	else
	{
		for (size_t i = 1; i < index.size(); i++)
			this->_index.push_back(index[i]);
	}
}

void	Directives::setHttpRedir(std::vector<std::string> redir, int *i) {
	if (redir.size() != 2)
		error_line(*i, "directive return wrong format");
	else if (!this->_httpRedirSet)
	{
		this->_httpRedirSet = true;
		this->_httpRedir = redir[1];
	}
	else
		error_line(*i, "return is already set");
}

void	Directives::setHttpMethods(std::vector<std::string> methods, int *i) {
	if (methods.size() < 2)
		error_line(*i, "incorrect directive");
	else
	{
		for (size_t j = 1; j < methods.size(); j++)
		{
			if (methods[j] == "GET" or methods[j] == "HEAD" or methods[j] == "POST" or
				methods[j] == "DELETE")
			{
				bool	alreadySet = false;

				if (!this->_httpMethodsSet)
				{
					this->_httpMethodsSet = true;
					this->_httpMethods.clear();
				}
				for (size_t k = 1; k < this->_httpMethods.size(); k++)
				{
					if (this->_httpMethods[k] == methods[j])
						alreadySet = true;
				}
				if (!alreadySet)
					this->_httpMethods.push_back(methods[j]);
			}
			else
				error_line(*i, "incorrect HTTP methods");
		}
	}
}

void	Directives::setCgi(std::vector<std::string> cgi, int *i) {
	if (cgi.size()!= 3)
		error_line(*i, "Directive cgi, wrong args");
	else
		this->_cgi.insert(std::make_pair(cgi[1], cgi[2]));
}

// void	Directives::showErrorPage(std::ostream & o) const {
// 	if (this->getErrorPageSet())
// 	{
// 		o << "Error Page\t:";
// 		for (size_t i = 0; i < this->_errorPage.size(); i++)
// 			o << " (" << this->_errorPage[i].first << " " << this->_errorPage[i].second << ")";
// 	}
// 	o << std::endl;
// }

void	Directives::showErrorPage(std::ostream & o) const {
	if (this->getErrorPageSet())
	{
		o << "Error Page\t:";
		for (std::map<int, std::string>::const_iterator it = this->_errorPage.begin(); it != this->_errorPage.end(); it++)
			o << " (" << it->first << " " << it->second << ")";
	}
	o << std::endl;
}

void	Directives::showAutoindex(std::ostream & o) const {
	if (this->getAutoindexSet())
	{
		o << "Autoindex\t:";
		if (this->_autoindex)
			o << " on";
		else
			o << " off";
		o << std::endl;
	}
}

void	Directives::showIndex(std::ostream & o) const {
	if (this->getIndexSet())
	{
		o << "Index\t\t:";
		for (size_t i = 0; i < this->_index.size(); i++)
			o << " " << this->_index[i];
		o << std::endl;
	}
}

bool	Directives::checkFormatDir(std::vector<std::string> host, int *i) {
	std::string	tmp = host[host.size() - 1];
	int			len = tmp.size();

	if (tmp[len - 1] != ';')
	{
		std::cout << "Error: at line " << *i << " directive must be terminated by \';\'" << std::endl;
		return false;
	}
	return true;
}