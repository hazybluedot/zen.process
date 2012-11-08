#ifndef _ZEN_PROCESS_UTILS_HPP_
#define _ZEN_PROCESS_UTILS_HPP_

#include <string>
#include <vector>
#include <map>

#include "types.hpp"

namespace zen {
  namespace process {
    typedef std::vector<const char*> vecchar;
    typedef std::vector<std::string> vecstr;
    
    const char *convert(const std::string & s);
    std::vector<const char*> convert_vs2vc(const std::vector<std::string>& vs);
    argov_type process_options(const argv_type&, const opts_type&);
    void get_status(pid_t);

    std::ostream& operator<<(std::ostream& os, const vecstr& args);
    std::ostream& operator<<(std::ostream& os, const vecchar& args);
    std::ostream& operator<<(std::ostream& os, const argv_type& argsv);
  }
}
#endif
