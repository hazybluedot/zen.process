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

#ifndef _ZEN_SELF_PIPE_TRICK_HPP_
#define _ZEN_SELF_PIPE_TRICK_HPP_

#include <cerrno>
#include <stdexcept>
#include <array>
#include <string>
#include <vector>

namespace zen {
  namespace process {
    class SelfPipeTrick
    {
      typedef std::runtime_error ExceptionT;
    public:
      SelfPipeTrick();
      ~SelfPipeTrick();

      void post_exec(int);
      void pre_exec();
      int parent(const std::vector<std::string>&);
      void execvp(const std::vector<std::string>&);
    private:
      std::array<char,3> status;
      int pipefds[2];    
    };
  }
}
#endif
