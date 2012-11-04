#ifndef ZEN_PROCESS_TYPES
#define ZEN_PROCESS_TYPES

#include <string>
#include <vector>
#include <map>

namespace zen {
  namespace process {
      typedef std::vector<std::string> args_type;
      typedef std::vector<args_type> argv_type;
      typedef std::map<std::string,std::string> opts_type;
      typedef std::pair<args_type, opts_type> argo_type;
      typedef std::vector<argo_type> argov_type;
  }
}

#endif
