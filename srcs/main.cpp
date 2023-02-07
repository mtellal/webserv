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
	Possible d'avoir dans le fichier de conf 2 bloc server avec la meme directive listen
	mais a departager avec server_name ?
	Ameliorer le header qui doit etre similaire a celui de nginx
*/