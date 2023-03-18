#include "Configuration.hpp"
#include "SocketServer.hpp"

int main(int argc, char **argv, char **envp)
{
	std::string	file;

	if (argc != 2)
	{
		std::cout << "Error: 1 config file expected" << std::endl;
		return 1;
	}

	file = argv[1];
	
	Configuration	conf(file);
	if (conf.getErrorConf())
		return 1;

	SocketServer	serv(conf, envp);
	if (serv.getErrSocket())
		return 1;

	return 0;
}