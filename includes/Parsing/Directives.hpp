#ifndef DIRECTIVES_HPP
# define DIRECTIVES_HPP

#include "utils.hpp"
#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <ostream>
#include <fstream>
#include <cctype>
#include <algorithm>


class Directives {

	public:

		Directives();
		Directives(Directives const &src);
		virtual ~Directives();

		Directives	&operator=(const Directives &rhs);

		std::map<int, std::string>			getErrorPage() const;
		int									getClientMaxBodySize() const;
		std::string							getRoot() const;
		bool								getAutoindex() const;
		std::vector<std::string>			getIndex() const;
		std::string							getHttpRedir() const;
		std::vector<std::string>			getHttpMethods() const;
		bool								getErrorDirectives() const;
		std::map<std::string, std::string>	getCgi() const;
		std::string							getUpload() const;

		void								setErrorPage(std::vector<std::string> str, int &i);
		void								setClientMaxBodySize(std::vector<std::string> maxClient, int &i);
		void								setRoot(std::vector<std::string> root, int &i);
		void								setAutoindex(std::vector<std::string> autoindex, int &i);
		void								setIndex(std::vector<std::string> index, int &i);
		void								setHttpRedir(std::vector<std::string> redir, int &i);
		void								setHttpMethods(std::vector<std::string> redir, int &i);
		void								setCgi(std::vector<std::string> cgi, int &i);
		void								setUpload(std::vector<std::string> upload, int &i);

		bool								getErrorPageSet() const;
		bool								getClientMaxBodySizeSet() const;
		bool								getRootSet() const;
		bool								getAutoindexSet() const;
		bool								getIndexSet() const;
		bool								getHttpRedirSet() const;
		bool								getHttpMethodsSet() const;
		bool								getUploadSet() const;

		void								showErrorPage(std::ostream & o) const;
		void								showAutoindex(std::ostream & o) const;
		void								showIndex(std::ostream & o) const;

	protected:

		std::map<int, std::string>			_errorPage;
		int									_clientMaxBodySize;
		std::string							_root;
		bool								_autoindex;
		std::vector<std::string>			_index;
		std::string							_httpRedir;
		std::map<std::string, std::string>	_cgi;
		bool								_errorPageSet;
		bool								_clientMaxBodySizeSet;
		bool								_rootSet;
		bool								_autoindexSet;
		bool								_indexSet;
		bool								_httpRedirSet;
		bool								_errorDirectives;
		std::vector<std::string>			_httpMethods;
		bool								_httpMethodsSet;
		std::string							_upload;
		bool								_uploadSet;

		bool								checkFormatDir(std::vector<std::string> host, int &i);


	private:

		void								error_line(const int &n_line, const std::string &err_msg);

};

#endif