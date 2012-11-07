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

#include <string.h>

#include <algorithm>
#include <iterator>
#include "utils.hpp"
#include <sys/wait.h>

namespace zen {
  namespace process {
    std::vector<const char*> convert_vs2vc(const std::vector<std::string>& vs)
    {
      std::vector<const char*> vc;
      std::transform(vs.begin(), vs.end(), std::back_inserter(vc), [](const std::string s)
		     {
		       return s.c_str();
		     } );
      vc.push_back( NULL );
      return vc;
    };

    std::ostream& operator<<(std::ostream& os, const std::vector<const char*>& args)
    {
    
      //for_each(args.begin(), args.end(), [&os]
      std::copy(args.begin(), args.end(), std::ostream_iterator<const char*>(os, "\n"));
      //os << const_cast<char**>( &args[0] );
      return os;
    };

    std::ostream& operator<<(std::ostream& os, const vecstr& args)
    {
    
      //for_each(args.begin(), args.end(), [&os]
      std::copy(args.begin(), args.end(), std::ostream_iterator<std::string>(os, "\n"));
      //os << const_cast<char**>( &args[0] );
      return os;
    };

    std::ostream& operator<<(std::ostream& os, const argv_type& argsv)
    {
      unsigned int nn=0;
      for_each(argsv.begin(), argsv.end(), [&nn,&os](vecstr args)
	       {
		 os << "argsv[" << nn++ << "]:" << std::endl;
		 os << args << std::endl;
	       }
	       );

      return os;
    }

    std::vector<std::string> get_log_string(const std::string &procname, const std::string &name, opts_type &options)
    {
      std::vector<std::string>  logger;
      opts_type::const_iterator oit = options.find(name);
      if (oit != options.end())
	{
	  std::string logname = oit->second;
	  if (logname == "")
	    {
	      logname = procname;
	    }
	  logger = {"tee", "-a", logname + "."+ oit->first};
	  options.erase(oit);
	}
      return logger;
    };

    argov_type process_options(const argv_type &args_, const opts_type &options_)
    {
      argov_type argsv;

      std::for_each(args_.begin(), args_.end(), [&](const std::vector<std::string> &args) 
		    {
		      opts_type options = options_;
		      std::string ids = "";
		      std::vector<std::string> inlogger;
		      std::vector<std::string> outlogger;

		      std::map<std::string,std::string>::const_iterator oit;
		      oit = options.find("id");
		      if (oit != options.end())
			{
			  ids = oit->second;
			}
		      
		      std::string bname = basename(args[0].c_str()) + ids;
		      inlogger = get_log_string(bname, "stdin", options);
		      outlogger = get_log_string(bname, "stdout", options);
		  
		      if (inlogger.size() > 0)
			argsv.push_back(std::make_pair(inlogger, opts_type()));

		      argsv.push_back(std::make_pair(args, options));

		      if (outlogger.size() > 0)
			argsv.push_back(std::make_pair(outlogger, opts_type()));
		  
		    } );
      return argsv;
    }
    
    void get_status(pid_t pid_) {
      int status(0);
      pid_t pid(-1);
      while(-1 == (pid = waitpid(pid_, &status, 0)) && EINTR == errno);
      if(pid == pid_) {
	std::cerr << "child process " << pid;
	if (WIFEXITED(status)) {
	  std::cerr << " exited with return code " << WEXITSTATUS(status);
	} else if (WIFSIGNALED(status)) {
	  std::cerr << " exited via signal " << WTERMSIG(status) << " (" << strsignal(WTERMSIG(status)) << ")";
	} else {
	  std::cerr << " exited with status " << status;
	}
	std::cerr << std::endl;
      }
      else
	std::cerr << "child process " << pid_ 
		  << " had already exited" << std::endl;
    }
  }
}
