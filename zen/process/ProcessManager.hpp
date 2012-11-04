#ifndef ZEN_PROCESS_MANAGER
#define ZEN_PROCESS_MANAGER

#include <memory>
#include "types.hpp"
#include "PipelineIO.hpp"

namespace zen {
  namespace process {
    class Pipeline;

    class ProcessManager {
    public:
      typedef PipelineIO value_type;
      ProcessManager(bool verbose=false);
      virtual ~ProcessManager();
      PipelineIO spawn(const argv_type&, const opts_type &opts=opts_type());

    private:
      bool verbose;
      std::vector<std::shared_ptr<Pipeline> > m_pipelines;
      std::map<std::string, int> m_names;
    };
  }
}

#endif
