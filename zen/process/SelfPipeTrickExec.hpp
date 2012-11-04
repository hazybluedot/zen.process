#ifndef ZEN_PROCESS_SELFPIPETRICKEXEC_HPP_
#define ZEN_PROCESS_SELFPIPETRICKEXEC_HPP_

#include "Pipe.hpp"

namespace zen {
namespace process {

/**
 * @class SelfPipeTrickExec
 * This class is based on the adapation of the self-pipe trick
 * http://cr.yp.to/docs/selfpipe.html
 * http://stackoverflow.com/questions/1584956/how-to-handle-execvp-errors-after-fork
 */
class SelfPipeTrickExec
{
	// Methods
public:
	explicit SelfPipeTrickExec();
	~SelfPipeTrickExec();
	void writeExecStatus(const int& status);
	int checkExecStatus();

	// Implementation
private:
	Pipe p_;
};

} // end namespace process
} // end namespace zen

#endif // ZEN_PROCESS_SELFPIPETRICKEXEC_HPP_
