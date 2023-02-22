#include "Configuration.hpp"
#include "SocketServer.hpp"
#include "CGI.hpp"

int main(int argc, char **argv, char **envp)
{
	std::string	file;
 
	(void)argv;
	(void)envp;

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

	SocketServer	serv(conf, envp);
	if (serv.getErrSocket())
		return 1;

	return 0;
}

/*
	Pour get et post, checker le format des args envoyes
	Voir 4096

	Method HEAD ?

	Plusieurs fois le meme port dans le fichier conf ?
	-> Msg envoye.

	Fichier Response.cpp a clarifier

	Tester le code 406

	favicon ?
*/