/* Copyright (C) 2012 Darren Maczka <darmacz@gmail.com>
   
   This file is part of zen.process.
   
   zen.process is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.
   
   zen.process is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.
   
   You should have received a copy of the GNU General Public License
   along with zen.process.  If not, see <http://www.gnu.org/licenses/>.
*/

#include <iostream>
#include "zen/process/ProcessManager.hpp"

int main(int argc, char* argv[])
{
	namespace zp = zen::process;
	zp::argv_type cv;
	cv.push_back({"./dummy", "10"});
	cv.push_back({"./dummy", "15"});
	zp::ProcessManager pm(true);
	std::map<std::string, std::string> opts;
	opts["stdin"] = ""; //Seting these keys to the empty string tells Pipeline to use the first process name + id as the filename followed by .key
	opts["stdout"] = ""; // key is the key used
	opts["stderr"] = ""; // e.g. dummy0.stderr for the first pipeline, dummy1.stderr for the second, etc;
	// Currently counting is handed by the process manager, so in the case of a pipeline containing multiple processes with the same name, each of their standard streams will be logged to the *same* file. This should probably be changed so that Pipline appends its own index if there are more than one processes with the same name, e.g. dummy0.0.stderr, dummy0.1.stderr
	zp::ProcessManager::value_type pipeline = pm.spawn(cv, opts);
	zp::ProcessManager::value_type pipeline2 = pm.spawn(cv, opts);

	std::string poke("this is from pipeline write\n");

	std::cout << std::endl << "[Pipeline] wrote: " << poke << std::endl;
	pipeline.write(poke);
	pipeline2.write(poke);
	std::string response(pipeline.read());
	std::cout << "[Pipeline] read: " << response;
	std::cout << std::endl;

	std::string response2(pipeline2.read());
	std::cout << "[Pipeline2] read: " << response2;
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
