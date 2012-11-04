#ifndef ZEN_PROCESS_PIPELINE_IO
#define ZEN_PROCESS_PIPELINE_IO

#include <string>
#include <memory>

#include "Pipeline.hpp"

namespace zen {
  namespace process {
    class PipelineIO
    {
    public:
      PipelineIO(std::shared_ptr<Pipeline> &pl) : m_pipeline(pl) {}
      virtual ~PipelineIO();
      void write(const std::string&);
      std::string read();
      void stop();
      void cont();
    private:
      std::shared_ptr<Pipeline> m_pipeline;
    };
  }
}

#endif
