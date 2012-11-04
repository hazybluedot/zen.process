#ifndef ZEN_PROCESS_MANAGER
#define ZEN_PROCESS_MANAGER

#include <memory>
#include "types.hpp"
#include "PipelineIO.hpp"

namespace zen {
  namespace process {
    class Pipeline;

    class ProcessManager {
      typedef std::map<std::string, int> name_map;
    public:
      typedef PipelineIO value_type;
      ProcessManager(bool verbose=false);
      virtual ~ProcessManager();
      PipelineIO spawn(const argv_type&, const opts_type &opts=opts_type());

    private:
      bool verbose;
      std::vector<std::shared_ptr<Pipeline> > m_pipelines;
      name_map m_names;
    };
  }
}

#endif
