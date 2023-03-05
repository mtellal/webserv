#ifndef UTILS_HPP
# define UTILS_HPP

# include <string>
# include <vector>
# include <fstream>

int							ft_stoi(const std::string str, bool *err);
bool						only_space_or_empty(std::string line);
size_t                      tab_len(char **env);
std::string					ft_itos(int nbr);
std::string					getHttpStatusCodeMessage(int statusCode);
std::string					fileToStr(std::string path);
std::string 	            strtrim_char(std::string s, char c);
std::vector<std::string>	ft_split_str(const std::string &s, const std::string &str);
std::vector<std::string>	ft_split(const std::string &str, const std::string &charset);
bool	                    infileExists(const std::string &file);


#endif