#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <iostream>
#include <algorithm>
#include "AgentList.hpp"

AgentList::AgentList(size_t N, const std::vector<char*>& args, bool verbose) : verbose(verbose) {
    for(size_t nn=0; nn<N; ++nn)
	execute(args); //TODO: What happens if execute returns an error?
};

AgentList::~AgentList()
{
    //std::map<pid_t,int>::const_iterator pidit = m_pids.begin();
    int status;
    close_all();
    while (m_pids.size() > 0)
    {
	pid_t pid = wait(&status);
	if (pid < 0)
	{ /* some error */
	    perror("wait");
	} else {
	    /* process is done, remove from map */
	    if (verbose)
		std::cerr << "Child process " << pid << " terminated with status " << status << std::endl;
	    m_pids.erase(pid);
	}
    }
};

void AgentList::close_all() {
    std::for_each(m_pids.begin(), m_pids.end(), [&](std::pair<pid_t,int> pp){ close(pp.second); } );
}

void AgentList::execute(const std::vector<char*>& agent_args) {
    
    int fd[2];
    pipe(fd);

    pid_t pid = fork();

    if (pid == -1)
    {
	perror("fork");
	_exit(1);
	//throw an exception?
    } else if (pid != 0)
    {
	/* parent process */
	close(fd[0]);
	m_pids.insert(value_type(pid,fd[1]));
	if (verbose)
	    std::cerr << "Parent: adding child PID: " << pid << std::endl;
    }
    else if (pid == 0)
    {
	/* child process */
	close(fd[1]);
	dup2(fd[0], 0); /* close default dup fd[0] to 0 */
	if (verbose)
	    std::cerr << "Child: exec'ing " << agent_args[0] << std::endl;
	execv(agent_args[0], const_cast<char**>( &agent_args[0] ) );
	std::cerr << "Major error, wtf!" << std::endl;
	_exit(1);
    }
}
