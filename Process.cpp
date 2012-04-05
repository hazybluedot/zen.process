#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>
#include "Process.hpp"

Process::Process(const std::vector<char*>& exec_args)
{
    int fd[2];
    pipe(fd);
    m_pid = fork();

    if (m_pid < 0)
    {
	perror("fork");
	throw std::string("Could not fork process");
    } else if (m_pid != 0)
    {
	/* parent process */
	close(fd[0]);
	m_pout = fdopen(fd[1], "w");
	std::string line = "test\n";
	fprintf(m_pout, line.c_str());
	fprintf(m_pout, line.c_str());
	fprintf(m_pout, line.c_str());
	std::cerr << "Opened stream to child " << m_pid << std::endl;
    } else if (m_pid == 0)
    {
	close(fd[1]);
	dup2(fd[0], 0);
	execv(exec_args[0], const_cast<char**>( &exec_args[0] ) );
	//close(m_fd[0]);
	throw std::string("Could not execv process");
    }
};

Process::~Process()
{
    int status;
    //close(m_fd[1]);
    pid_t pid = wait(&status);
    if (pid < 0)
    {
	perror("wait");
    }
};

std::ostream &operator<<(std::ostream& os, const Process &proc)
{
    
}

std::istream &operator>>(std::istream& is, Process &proc)
{
    std::string line;
    is >> line;
    if (proc.m_pout > 0)
    {
	std::cerr << "writing line to process " << proc.m_pid << std::endl;
	std::cerr << line;
	std::cerr << "END LINE" << std::endl;
	fprintf(proc.m_pout, line.c_str());
    }
}
