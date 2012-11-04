#include <iostream>
#include "zen/process/ProcessManager.hpp"

int main(int argc, char* argv[])
{
	namespace zp = zen::process;
	zp::argv_type cv;
	cv.push_back({"./dummy", "10"});
	cv.push_back({"./dummy", "15"});
	zp::ProcessManager pm(true);
	zp::ProcessManager::value_type pipeline = pm.spawn(cv);

	std::string poke("this is from pipeline write\n");

	std::cout << std::endl << "[Pipeline] wrote: " << poke << std::endl;
	pipeline.write(poke);
	std::string response(pipeline.read());
	std::cout << "[Pipeline] read: " << response;
	std::cout << std::endl;

	/*
	std::cout << "[Pipeline] checking for broken pipeline "
	          << pipeline.isBroken() 
	          << " (expected = 0)" << std::endl;
	*/
	sleep(20);
	std::cout << "[Pipeline] shutting down" << std::endl;

	return 0;
}
