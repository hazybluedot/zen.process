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
