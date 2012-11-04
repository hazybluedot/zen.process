#ifndef ZEN_PROCESS_PIPE_HPP_
#define ZEN_PROCESS_PIPE_HPP_

#include <unistd.h>
#include <iostream>

namespace zen {
namespace process {

/**
 * @class Pipe
 * Pipe class is utility class to hold file descriptors of a pipe.
 * Its constructor creates a new pipe, however the destructor does not close that pipe.
 * It is the responsibility of the user to properly close all file descriptors.
 */
class Pipe
{
	// Enums
public:
	enum CloseOnExec
	{
		CLOSE_ON_EXEC = true,
		NO_CLOSE_ON_EXEC = false
	};

	// Methods
public:
	explicit Pipe();
	explicit Pipe(const CloseOnExec& close_on_exec);

	int readfd() const;
	int writefd() const;

	void close();
	void closeRead();
	void closeWrite();

	// Implementation
protected:
	int fd_[2];
};

inline int Pipe::readfd() const
{
	return fd_[0];
}

inline int Pipe::writefd() const
{
	return fd_[1];
}

inline void Pipe::closeRead()
{
	::close(fd_[0]);
}

inline void Pipe::closeWrite()
{
	::close(fd_[1]);
}

} // end namespace process
} // end namespace zen

#endif // ZEN_PROCESS_PIPE_HPP_
