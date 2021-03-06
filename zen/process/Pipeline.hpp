/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef _ZEN_PROCESS_PIPELINE_HPP_
#define _ZEN_PROCESS_PIPELINE_HPP_

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <sys/types.h>
#include <vector>
#include <string>
#include <memory>

#ifdef HAVE_JSONCPP
#include <json/json.h>
#endif

#include "types.hpp"
#include "Pipe.hpp"

namespace zen {
  namespace process {
    class Pipeline
    {
    public:
      typedef std::pair<pid_t, std::string> value_type;

    public:
      Pipeline(const args_type& argsv, const bool verbose = false, const opts_type& options = opts_type());
      Pipeline(const argv_type& argsv, const bool verbose = false, const opts_type& options = opts_type());
      ~Pipeline();
      // non-copyable
      Pipeline(const Pipeline&) = delete;
      Pipeline& operator=(const Pipeline&) = delete;
  
      void write(const std::string& line);
      std::string read() const;
      std::string readline() const;
  
      void stop();
      void cont();
    
#ifdef HAVE_JSONCPP
      void write(const Json::Value& jvalue);
      Json::Value read_json() const;
#endif

    private:
      bool verbose;
      std::string m_name;
      std::vector<value_type> m_processes;
      FILE* m_pread;
      FILE* m_pwrite;

      void spawn(const argov_type&);
      value_type exec(const argo_type&, Pipe&, Pipe&);
      void validate();
    };

  }
}

#endif
