#ifndef UTILS_HPP
# define UTILS_HPP

# include <string>
# include <vector>

int							ft_stoi(const std::string str, bool *err);
bool						only_space_or_empty(std::string line);
std::vector<std::string>	ft_split(const std::string &str, const std::string &charset);
std::string					ft_itos(int nbr);
std::string					getHttpStatusCodeMessage(int status);

#endif