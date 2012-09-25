#ifndef _ZEN_PROCESS_UTILS_HPP_
#define _ZEN_PROCESS_UTILS_HPP_

#include <string>
#include <vector>

typedef std::vector<std::string> vecstr;
typedef std::vector<vecstr> vecvecstr;
typedef std::vector<const char*> vecchar;

vecvecstr add_output_logger(const vecvecstr& pipeline, const std::string& logname);
vecvecstr add_input_logger(const vecvecstr& pipeline, const std::string& logname);
std::string basename(const vecstr& args);

const char *convert(const std::string & s);
std::vector<const char*> convert_vs2vc(const std::vector<std::string>& vs);

std::ostream& operator<<(std::ostream& os, const vecstr& args);
std::ostream& operator<<(std::ostream& os, const vecchar& args);
std::ostream& operator<<(std::ostream& os, const vecvecstr& argsv);

#endif
