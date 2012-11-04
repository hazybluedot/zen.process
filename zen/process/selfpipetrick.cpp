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

#include <fcntl.h>
#include <unistd.h>
#include <cerrno>
#include <sysexits.h>
#include <cstring>

#include <iostream>
#include <algorithm>
#include <iterator>
#include <sstream>

#include "selfpipetrick.hpp"
#include "utils.hpp"

namespace zen {
  namespace process {
    SelfPipeTrick::SelfPipeTrick() : status{{-1,-1,-1}} {

      /* self-pipe trick: http://cr.yp.to/docs/selfpipe.html
	 implementation example: http://stackoverflow.com/questions/1584956/how-to-handle-execvp-errors-after-fork */
      if (pipe(pipefds)) {
	perror("pipe");
	throw ExceptionT("pipe");
	//return EX_OSERR;
      }
      if (fcntl(pipefds[1], F_SETFD, fcntl(pipefds[1], F_GETFD) | FD_CLOEXEC)) {
	perror("fcntl");
	close(pipefds[0]);
	close(pipefds[1]);
	throw ExceptionT("fcntl");
	//return EX_OSERR;
      }
      status[0] = 0;
      /* end self-pipe trick */
    }

    SelfPipeTrick::~SelfPipeTrick()
    {
      if (std::min_element(status.begin(),status.end()) < 0)
	std::cerr << "Warning: SelfPipeTrick going out of scope before it has been used" <<std::endl;
    }

    void SelfPipeTrick::post_exec(int err)
    {
      if (status[2] < 0)
	{
	  ::write(pipefds[1], &err, sizeof(int)); //self-pipe trick
	  status[2] = 0;
	  _exit(0);
	} else {
	throw std::logic_error("SelfPipeTrick post_exec called more than once");
      };
    }

    void SelfPipeTrick::pre_exec()
    {
      if (status[1] < 0)
	close(pipefds[0]); //self-pipe trick
      else
	throw std::logic_error("SelfPipeTrick pre_exec called more than once");
    }

    void SelfPipeTrick::execvp(const std::vector<std::string>& args)
    {
      std::vector<const char*> vc = convert_vs2vc(args);
      pre_exec();
      /*
	std::cerr << "Exec args: ";
	std::copy(vc.begin(), vc.end(), std::ostream_iterator<const char*>(std::cerr, ", "));
	std::cerr << std::endl;
      */
      ::execvp(vc[0], const_cast<char**>( &vc[0] ) );
      post_exec(errno);
    }

    int SelfPipeTrick::parent(const std::vector<std::string>& args)
    {
      int count, err;
    
      if (status[0] == 0 && status[1] < 0 && status [2] < 0)
	{
	  /* self-pipe trick parent side */
	  close(pipefds[1]);
	  while ((count = ::read(pipefds[0], &err, sizeof(errno))) == -1)
	    if (err != EAGAIN && err != EINTR) break;
	  if (count) {
	    std::string error;
	    {
	      std::ostringstream oss;
	      oss <<  "child's execvp [" << args[0] << "]: " << strerror(err) << "\n";			 
	      error = oss.str();
	    }
	    throw std::runtime_error(error);
	  }
	  close(pipefds[0]);
	  /* end self-pipe trick parent side */
	  return 0;
	} else {
	close(pipefds[0]);
	close(pipefds[1]);
	throw std::logic_error("SelfPipeTrick parent called at the wrong state");
      }
    }
  }
}
