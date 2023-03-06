#ifndef LOCATION_HPP
# define LOCATION_HPP

# include "Directives.hpp"

class Location : public Directives {

	public:

		Location();
		Location(int &i, std::vector<std::string> loc);
		Location(Location const &src);
		virtual ~Location();

		Location	&operator=(Location const &rhs);

		
		void							readLocationBlock(std::ifstream &file, int &i);
		
		bool							getErrorLoc();
		
		std::string						getPath();

		protected:
		
			bool						_errorLoc;
			std::string					_path;


	private:

		void	(Location::*functPtr[9])(std::vector<std::string>, int &i);
		void	setPath(int &i, std::string loc);
		void	error_line(const int &n_line, const std::string &err_msg);
};

#endif