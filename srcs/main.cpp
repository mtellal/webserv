#include "../includes/Configuration.hpp"
#include "../includes/SocketServer.hpp"

int main(int argc, char **argv)
{
	std::string	file;

	if (argc > 2)
	{
		std::cout << "Error: 2 args maximum expected" << std::endl;
		return 1;
	}
	file = "configuration_file/default.conf";
	if (argc == 2)
		file = argv[1];
	Configuration	conf(file);
	// std::vector<Server> test = conf.getVctServer();
	if (conf.get_errorConf())
		return 1;
	// std::cout << conf << std::endl;
	SocketServer	serv(conf);
	if (serv.getErrSocket())
		return 1;
	return 0;
}

/*
	Possible d'avoir dans le fichier de conf 2 bloc server avec le meme port et
	donc a departager avec server_name ?
	Http Methods uniquement dans bloc location ou aussi dans bloc Server ?

	Types MIME
	https://www.w3.org/Protocols/rfc2616/rfc2616.html

	Mime: https://wiki.debian.org/fr/MIME
	ET https://www.w3.org/Protocols/rfc2616/rfc2616-sec14.html
*/