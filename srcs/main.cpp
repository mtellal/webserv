#include "Configuration.hpp"
#include "SocketServer.hpp"

int main(int argc, char **argv, char **envp)
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
	if (conf.getErrorConf())
		return 1;

	// std::cout << conf << std::endl;
	// (void)envp;

	SocketServer	serv(conf, envp);
	if (serv.getErrSocket())
		return 1;

	return 0;
}

/*
	-Check les valeur de read dans les pipes

	-Leaks avec form

	-Si on essaye d'upolad un fichier mais que la direrctive n'est pas set
	dans le fichier de conf => Err 500 et modifier le message affiche dans le
	terminal.

	send call failed: Broken pipe
*/
