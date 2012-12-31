/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include <errno.h>
#include <unistd.h>

#include "SelfPipeTrickExec.hpp"

namespace zen {
namespace process {

SelfPipeTrickExec::SelfPipeTrickExec()
	: p_(Pipe(Pipe::CLOSE_ON_EXEC))
{}

SelfPipeTrickExec::~SelfPipeTrickExec()
{
	p_.close();
}

void SelfPipeTrickExec::writeExecStatus(const int& status)
{
	p_.closeRead();
	::write(p_.writefd(), &status, sizeof(int)); //self-pipe trick
}

int SelfPipeTrickExec::checkExecStatus()
{
	p_.closeWrite();

	int count(0), status(0);
	while(-1 == (count = ::read(p_.readfd(), &status, sizeof(errno))) 
	      && (EINTR == errno || EAGAIN == errno));
	return (count == 0) ? 0 : status;
}

} // end namespace process
} // end namespace zen
