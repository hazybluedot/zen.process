/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <iostream>
#include "../zen/process/Pipeline.hpp"

#ifdef HAVE_JSONCPP
#include <json/json.h>
#else
#warning "No jsoncpp"
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
