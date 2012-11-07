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

#include "ProcessManager.hpp"
#include <algorithm>
#include <string.h>

#include "types.hpp"
#include "utils.hpp"
#include "ProcessManager.hpp"

#include "Pipeline.hpp"
#include "PipelineIO.hpp"
#include <iostream>
#include <sstream>

namespace zen {
  namespace process {
    ProcessManager::ProcessManager(bool verbose) : verbose(verbose) {};
    ProcessManager::~ProcessManager() {
      /* do we actually have to do anything here?
      std::for_each(m_pipelines.begin(), m_pipelines.end(), [](std::shared_ptr<Pipeline> &pl)
		    {
		      pl->continue();
		      pl->terminate();
		    } );
      */
    };

    /* spawn a new pipeline
     */
    PipelineIO ProcessManager::spawn(const argv_type &args, const opts_type &options) 
    {
      opts_type opts = options;
      std::string name = basename(args[0][0].c_str());
      name_map::iterator it = m_names.find(name);
      
      if (it != m_names.end()) {
	++((*it).second);
      } else {
	m_names[name] = 0;
      }
      std::stringstream ss;
      ss << m_names[name];
      opts["id"] = ss.str(); //m_names.find(name)->second;
      std::shared_ptr<Pipeline> pipeline = std::shared_ptr<Pipeline>(new Pipeline(args, verbose, opts));
      m_pipelines.push_back(pipeline);
      if (verbose)
	std::cerr << "Adding pipeline " << name << opts["id"] << "\n";
      return PipelineIO(pipeline);
    };
  }
}
