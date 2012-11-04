#include "ProcessManager.hpp"
#include <algorithm>

#include "types.hpp"
#include "ProcessManager.hpp"

#include "Pipeline.hpp"
#include "PipelineIO.hpp"
#include <iostream>

namespace zen {
  namespace process {
    ProcessManager::ProcessManager(bool verbose) : verbose(verbose) {};
    ProcessManager::~ProcessManager() {
      /* do we actually have to do anything here?
      std::for_each(m_pipelines.begin(), m_pipelines.end(), [](std::shared_ptr<Pipeline> &pl)
		    {
		      pl->continue();
		      pl->terminate();
		    } );
      */
    };

    /* spawn a new pipeline
     */
    PipelineIO ProcessManager::spawn(const argv_type &args, const opts_type &options) 
    {
      std::shared_ptr<Pipeline> pipeline = std::shared_ptr<Pipeline>(new Pipeline(args, verbose, options));
      m_pipelines.push_back(pipeline);
      if (verbose)
	std::cerr << "Adding pipeline...\n";
      return PipelineIO(pipeline);
    };
  }
}
