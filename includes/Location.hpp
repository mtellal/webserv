#ifndef LOCATION_HPP
# define LOCATION_HPP

# include "Directives.hpp"

class Location : public Directives {

	public:
	Location();
	Location(std::ifstream &file, int *i, std::vector<std::string> loc);
	Location(Location const &src);
	virtual ~Location();

	Location	&operator=(Location const &rhs);

	std::string						getCgi();
	std::string						getPath();
	bool							getErrorLoc();
	bool							getHttpMethodsSet();
	bool							getCgiSet();

	void							showHttpMethods(std::ostream &o);


	protected:
	std::string					_cgi;
	std::string					_path;
	bool						_cgiSet;
	bool						_errorLoc;


	private:
	void	(Location::*functPtr[8])(std::vector<std::string>, int *i);

	void	setPath(int *i, std::string loc);
	bool	charAccepted(char c);
	void	readBlock(std::ifstream &file, int *i);

	void	setCgi(std::vector<std::string> cgi, int *i);
};

#endif