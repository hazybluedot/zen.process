#include <stdexcept>
#include <unistd.h>
#include <fcntl.h>

#include <config.h>
#include "Pipe.hpp"

namespace zen {
namespace process {

Pipe::Pipe()
{
	if(0 != pipe(fd_))
	{
		this->close(); // not sure if call to close matters here
		throw std::runtime_error("could not create pipe");
	}
}

Pipe::Pipe(const CloseOnExec& close_on_exec)
{
#ifdef HAVE_PIPE2

	if(0 != pipe2(fd_, O_CLOEXEC))
	{
		this->close(); // not sure if call to close matters here
		throw std::runtime_error("could not create pipe");
	}

#else

	if(0 != pipe(fd_))
	{
		this->close(); // not sure if call to close matters here
		throw std::runtime_error("could not create pipe");
	}

	if(CLOSE_ON_EXEC == close_on_exec)
	{
		if(-1 == fcntl(readfd(), F_SETFD, FD_CLOEXEC) ||
		   -1 == fcntl(writefd(), F_SETFD, FD_CLOEXEC))
		{
			this->close();
			throw std::runtime_error("could not set FD_CLOEXEC on pipe");
		}
	}

#endif
}

void Pipe::close()
{
	closeRead();
	closeWrite();
}

} // end namespace process
} // end namespace zen
