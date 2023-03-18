#include "Directives.hpp"
#include "utils.hpp"


Directives::Directives() : 
_cgiSet(false), _rootSet(false), _indexSet(false), _uploadSet(false),
_autoindex(false), _errorPageSet(false), _autoindexSet(false), _httpRedirSet(false),
_httpMethodsSet(false), _errorDirectives(false), _clientMaxBodySizeSet(false),
_clientMaxBodySize(1), _root("html")
{
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


		this->_cgiSet = rhs._cgiSet;
		this->_rootSet = rhs._rootSet;
		this->_indexSet = rhs._indexSet;
		this->_autoindex = rhs._autoindex;
		this->_uploadSet = rhs._uploadSet;
	
		this->_autoindex = rhs._autoindex;
		this->_errorPageSet = rhs._errorPageSet;
		this->_autoindexSet = rhs._autoindexSet;
		this->_httpRedirSet = rhs._httpRedirSet;
		this->_httpMethodsSet = rhs._httpMethodsSet;
		this->_errorDirectives = rhs._errorDirectives;
		this->_clientMaxBodySizeSet = rhs._clientMaxBodySizeSet;

		this->_clientMaxBodySize = rhs._clientMaxBodySize;

		this->_root = rhs._root;
		this->_upload = rhs._upload;
		this->_httpRedir = rhs._httpRedir;

		this->_index = rhs._index;
		this->_httpMethods = rhs._httpMethods;

		this->_errorPage = rhs._errorPage;
		this->_cgi = rhs._cgi;
	}
	return *this;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//												G E T T E R													  //
////////////////////////////////////////////////////////////////////////////////////////////////////////////////


bool								Directives::getCgiSet() const { return this->_cgiSet; }
bool								Directives::getRootSet() const { return this->_rootSet; }
bool								Directives::getIndexSet() const { return this->_indexSet; }
bool								Directives::getAutoindex() const { return this->_autoindex; }
bool								Directives::getUploadSet() const { return this->_uploadSet; }
bool								Directives::getAutoindexSet() const { return this->_autoindexSet; }
bool								Directives::getErrorPageSet() const { return this->_errorPageSet; }
bool								Directives::getHttpRedirSet() const { return this->_httpRedirSet; }
bool								Directives::getHttpMethodsSet() const { return this->_httpMethodsSet; }
bool								Directives::getErrorDirectives() const {return this->_errorDirectives; }
bool								Directives::getClientMaxBodySizeSet() const { return this->_clientMaxBodySizeSet; }

int									Directives::getClientMaxBodySize() const { return this->_clientMaxBodySize; }

std::string							Directives::getRoot() const { return this->_root; }
std::string							Directives::getUpload() const { return this->_upload; }
std::string							Directives::getHttpRedir() const { return this->_httpRedir; }

std::vector<std::string>			Directives::getIndex() const { return this->_index; }
std::vector<std::string>			Directives::getHttpMethods() const { return this->_httpMethods; }

std::map<int, std::string>			Directives::getErrorPage() const { return this->_errorPage; }
std::map<std::string, std::string>	Directives::getCgi() const { return this->_cgi; }

////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//												S E T T E R													  //
////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// A MODIFIER
void	Directives::setErrorPage(std::vector<std::string> str, int &i) {
	bool	err = false;
	int		nbError;

	if (str.size() != 3 || (str.size() == 3 && (str[2] == ";" || str[2].empty())))
		return (error_line(i, "directive error_page, wrong format"));
	nbError = ft_stoi(str[1], &err);
	if (err)
		error_line(i, "code error_page is incorrect");
	else if ((nbError >= 400 and nbError <= 418) or (nbError >= 421 and nbError <= 426)
		or (nbError >= 428 and nbError <= 429) or nbError == 431 or (nbError >= 449 and nbError <= 451)
		or nbError == 456)
	{
		if (!this->_errorPageSet)
			this->_errorPageSet = true;
		this->_errorPage[nbError] = str[2];
	}
	else
		error_line(i, "code error_page don't exist");
}

void	Directives::setClientMaxBodySize(std::vector<std::string> maxClient, int &i) {
	bool	err = false;
	int		nbMaxClient;

	if (maxClient.size() != 2 || (maxClient.size() == 2 && (maxClient[1] == ";" || maxClient[1].empty())))
		return (error_line(i, "directive client_max_body_size, wrong format"));
	nbMaxClient= ft_stoi(maxClient[1], &err);
	if (err)
		error_line(i, "client_max_body_size must be contains only numeric values");
	if (nbMaxClient >= 0)
	{
		if (!this->_clientMaxBodySizeSet)
		{
			this->_clientMaxBodySizeSet = true;
			this->_clientMaxBodySize = nbMaxClient;
		}
		else
			error_line(i, "client_max_body_size is already set");
	}
	else
		error_line(i, "client_max_body_size must be positive");
}

void	Directives::setRoot(std::vector<std::string> root, int &i) {
	if (root.size() != 2 || (root.size() == 2 && (root[1] == ";" || root[1].empty())))
		error_line(i, "directive root, wrong format");
	else if (!this->_rootSet)
	{
		this->_rootSet = true;
		this->_root = root[1];
	}
	else
		error_line(i, "root is already set");
}

void	Directives::setAutoindex(std::vector<std::string> autoindex, int &i) {
	if (autoindex.size() != 2 || (autoindex.size() == 2 && (autoindex[1] == ";" || autoindex[1].empty())))
		error_line(i, "directive autoindex, wrong format");
	else if (!this->_autoindexSet and (autoindex[1] == "on" or autoindex[1] == "off"))
	{
			this->_autoindexSet = true;
		if (autoindex[1] == "on")
			this->_autoindex = true;
		else
			this->_autoindex = false;
	}
	else if (!this->_autoindexSet)
		error_line(i, "directive autoindex, value must be on or off");
	else
		error_line(i, "autoindex is already set");
}

void	Directives::setIndex(std::vector<std::string> index, int &i) {
	if (this->_indexSet)
		error_line(i, "index is already set");
	else if (index.size() < 2 || index[index.size() - 1].empty())
		error_line(i, "directive index wrong format");
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

void	Directives::setHttpRedir(std::vector<std::string> redir, int &i) {
	if (redir.size() != 2 || (redir.size() == 2 && (redir[1] == ";" || redir[1].empty())))
		error_line(i, "directive return wrong format");
	else if (!this->_httpRedirSet)
	{
		this->_httpRedirSet = true;
		this->_httpRedir = redir[1];
	}
	else
		error_line(i, "return is already set");
}

void	Directives::setHttpMethods(std::vector<std::string> methods, int &i) {
	if (this->_httpMethodsSet)
		error_line(i, "http_methods is already set");
	else if (methods.size() < 2 || methods[methods.size() - 1].empty())
		error_line(i, "incorrect directive");
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
				error_line(i, "incorrect HTTP methods");
		}
	}
}

void	Directives::setCgi(std::vector<std::string> cgi, int &i) {
	if (cgi.size() != 3 || (cgi.size() == 3 && (cgi[2] == ";" || cgi[2].empty())))
		error_line(i, "Directive cgi, wrong args");
	else
	{
		this->_cgiSet = true;
		this->_cgi.insert(std::make_pair(cgi[1], cgi[2]));
	}
}

void	Directives::setUpload(std::vector<std::string> upload, int &i) {
	if (this->_uploadSet)
		error_line(i, "directive upload is already set");
	else if (upload.size() != 2 || (upload.size() == 2 && (upload[1] == ";" || upload[1].empty())))
		error_line(i, "Directive upload, wrong args");
	else
	{
		this->_upload = upload[1];
		this->_uploadSet = true;
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//										M E M B E R S   F U N C T I O N S 									  //
////////////////////////////////////////////////////////////////////////////////////////////////////////////////

bool	Directives::checkFormatDir(std::vector<std::string> host, int &i) {
	std::string	tmp = host[host.size() - 1];
	int			len = tmp.size();

	if (tmp[len - 1] != ';' || (len > 1 && tmp[len - 2] == ';'))
	{
		std::cout << "Error: at line " << i << " directive must be terminated by one \';\'" << std::endl;
		return false;
	}
	return true;
}

void	Directives::error_line(const int &n_line, const std::string &err_msg)
{
	this->_errorDirectives = true;
	std::cerr << "Error: at line " << n_line << " " << err_msg << std::endl;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//													D E B U G			 									  //
////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void	Directives::showErrorPage(std::ostream & o) const {
	if (this->getErrorPageSet())
	{
		o << "Error Page\t:";
		for (std::map<int, std::string>::const_iterator it = this->_errorPage.begin(); it != this->_errorPage.end(); it++)
			o << " (" << it->first << " " << it->second << ")";
		o << std::endl;
	}
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

void	Directives::showHttpMethods(std::ostream & o) const {
	if (this->getHttpMethodsSet())
	{
		o << "Http methods\t:";
		for (size_t i = 0; i < this->_httpMethods.size(); i++)
			o << " " << this->_httpMethods[i];
		o << std::endl;
	}
}

void	Directives::showCgi(std::ostream & o) const {
	if (this->getCgiSet())
	{
		o << "Cgi\t\t:";
		for (std::map<std::string, std::string>::const_iterator it = this->_cgi.begin(); it != this->_cgi.end(); it++)
			o << " (" << it->first << " " << it->second << ")";
		o << std::endl;
	}
}