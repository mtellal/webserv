#ifndef HEADER_HPP
# define HEADER_HPP

# include "Request.hpp"
# include "Server.hpp"
# include "Response.hpp"
# include <string>

class Response;

class Header {

	public:

		Header();
		Header(std::string file, int *statusCode, Response *rep);
		Header(std::string file, int *statusCode);
		Header(Header const &src);
		~Header();
	
		Header	&operator=(Header const &rhs);

		void	setContentType(std::string const &contentType);
		void	setContentLength(std::string const &contentLength);
		void	setStatus(int status);

		std::string	getHeader();
		std::string	getHeaderRequestError();
		std::string	getContentType();


	private:
	
		Request								_req;
		int									*_statusCode;
		std::string							_file;
		Server								_serv;
		Response							*_rep;
		std::map<std::string, std::string>	_header;

		std::string	ft_itos(int nbr) const;
		std::string	parsContentTypeFile(std::vector<std::string> splitAccept) const;

		std::string	getDate() const;
		std::string	getContentLength() const;
		std::string	getLastModified() const;
		std::string	getAllow() const;

};

#endif