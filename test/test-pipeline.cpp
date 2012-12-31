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
#include "../zen/process/Pipeline.hpp"

#ifdef HAVE_JSONCPP
#include <json/json.h>
#endif

int main(int argc, char* argv[])
{
  #ifdef HAVE_JSONCPP
  std::cout << "Testing with jsoncpp" << std::endl;
  Json::Value poke;
  #else
  std::string poke;
  #endif

	namespace zp = zen::process;
	zp::argv_type cv;
	cv.push_back({"./dummy", "10"});
	cv.push_back({"./dummy", "15"});
	zp::Pipeline pipeline(cv,true);

	std::string msg("this is from pipeline write\n");
#ifdef HAVE_JSONCPP
	poke["msg"] = msg;
	Json::FastWriter writer;
	std::cout << std::endl << "[Pipeline] wrote: " << writer.write(poke) << std::endl;
#else
	poke = msg;
	std::cout << std::endl << "[Pipeline] wrote: " << poke << std::endl;
#endif
	pipeline.write(poke);
	std::string response(pipeline.readline());
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
