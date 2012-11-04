#include <iostream>

#include <unistd.h>
#include <sys/types.h>

#include <cstdlib>

int main(int argc, char* argv[])
{
	std::string in;
	std::cerr << "dummy[" << getpid() << "] error stream" << std::endl;
	getline(std::cin, in);
	std::cout << "{ [" << getpid() << "] " << in << " }" << std::endl;
	int sleeptm(5);
	if(argc > 1)
		sleeptm = atoi(argv[1]);
	sleep(sleeptm);
	return 0;
}
