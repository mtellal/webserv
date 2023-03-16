#ifndef UTILS_HPP
# define UTILS_HPP

# include <string>
# include <vector>
# include <fstream>

class Server;
class Location;

void                        free_tab(char **tab);
void			            trimSpace(std::string &line);
void		                lowerCaseStr(std::string &str);
void						trimSpaceBack(std::string &line);
void						trimSpaceFront(std::string &line);
void						fdEpollout(int epollFd, int fd);
void						fdEpollin(int epollFd, int fd);
bool						only_space_or_empty(std::string line);
bool						infileExists(const std::string &file);
bool						resolveHost(const std::string& host, std::string& ipAddress);
int							ft_stoi(const std::string str, bool *err);
size_t						tab_len(char **env);
std::string					ft_itos(int nbr);
std::string					fileToStr(std::string path);
std::string 	            removeChar(std::string s, char c);
std::string		            getRightHost(const std::string& host);
std::string					getHttpStatusCodeMessage(int statusCode);
std::string					getIPFromHostName(const std::string& hostName);
std::string	                getHostNameFromIP(const std::string& ipAddress); 
std::vector<std::string>	ft_split_str(const std::string &s, const std::string &str);
std::vector<std::string>	ft_split(const std::string &str, const std::string &charset);


std::string					findRightPageError(int statusCode, Server const &serv, bool locBlockSelect, Location const &loc);
std::string					rightPathErr(bool &pageFind, int statusCode, Server const &serv, bool locBlockSelect, Location const &loc);
std::string					rightRoot(Server const &serv, bool locBlockSelect, Location const &loc);


#endif