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


		void								showCgi(std::ostream & o) const;
		void								showIndex(std::ostream & o) const;
		void								showErrorPage(std::ostream & o) const;
		void								showAutoindex(std::ostream & o) const;
		void								showHttpMethods(std::ostream & o) const;

		void								setCgi(std::vector<std::string> cgi, int &i);
		void								setRoot(std::vector<std::string> root, int &i);
		void								setIndex(std::vector<std::string> index, int &i);
		void								setUpload(std::vector<std::string> upload, int &i);
		void								setErrorPage(std::vector<std::string> str, int &i);
		void								setHttpRedir(std::vector<std::string> redir, int &i);
		void								setHttpMethods(std::vector<std::string> redir, int &i);
		void								setAutoindex(std::vector<std::string> autoindex, int &i);
		void								setClientMaxBodySize(std::vector<std::string> maxClient, int &i);

		bool								getCgiSet() const;
		bool								getRootSet() const;
		bool								getIndexSet() const;
		bool								getUploadSet() const;
		bool								getErrorPageSet() const;
		bool								getHttpRedirSet() const;
		bool								getAutoindexSet() const;
		bool								getHttpMethodsSet() const;
		bool								getClientMaxBodySizeSet() const;

		bool								getAutoindex() const;
		bool								getErrorDirectives() const;
		
		int									getClientMaxBodySize() const;
		
		std::string							getRoot() const;
		std::string							getUpload() const;
		std::string							getHttpRedir() const;
		
		std::vector<std::string>			getIndex() const;
		std::vector<std::string>			getHttpMethods() const;
		
		std::map<int, std::string>			getErrorPage() const;
		std::map<std::string, std::string>	getCgi() const;

	protected:

		bool								_cgiSet;
		bool								_rootSet;
		bool								_indexSet;
		bool								_uploadSet;
		bool								_autoindex;
		bool								_errorPageSet;
		bool								_autoindexSet;
		bool								_httpRedirSet;
		bool								_httpMethodsSet;
		bool								_errorDirectives;
		bool								_clientMaxBodySizeSet;

		int									_clientMaxBodySize;
		
		std::string							_root;
		std::string							_upload;
		std::string							_httpRedir;
		
		std::vector<std::string>			_index;
		std::vector<std::string>			_httpMethods;
		
		std::map<int, std::string>			_errorPage;
		std::map<std::string, std::string>	_cgi;

		bool								checkFormatDir(std::vector<std::string> host, int &i);


	private:

		void								error_line(const int &n_line, const std::string &err_msg);

};

#endif