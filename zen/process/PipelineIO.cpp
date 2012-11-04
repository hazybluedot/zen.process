#include "PipelineIO.hpp"

namespace zen {
  namespace process {
    PipelineIO::~PipelineIO() {};

    void PipelineIO::write(const std::string &msg) {
      m_pipeline->write(msg);
    };
    
    std::string PipelineIO::read()
    {
      return m_pipeline->read();
    };
    void stop();
    void cont();
    std::shared_ptr<Pipeline> m_pipeline;
  }
}

