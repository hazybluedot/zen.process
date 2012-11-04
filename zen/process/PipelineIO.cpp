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

#include "PipelineIO.hpp"

namespace zen {
  namespace process {
    PipelineIO::~PipelineIO() {};

    void PipelineIO::write(const std::string &msg) {
      m_pipeline->write(msg);
    };
    
    std::string PipelineIO::readline()
    {
      return m_pipeline->readline();
    };
    void stop(); //TODO: implement
    void cont(); //TODO: implement
    std::shared_ptr<Pipeline> m_pipeline;
  }
}

