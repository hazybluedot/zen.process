#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>
#include "Process.hpp"

Process::Process(const std::vector<char*>& exec_args) : verbose(false)
{
    //int fd[2];
    if ( pipe(m_fd) < 0)
    {
	perror("pipe");
	throw std::string("Could not create pipe");
    };

    if ( pipe(m_fd+2) < 0)
    {
	perror("pipe");
	throw std::string("Could not create pipe");
    };
    
    m_pid = fork();

    if (m_pid < 0)
    {
	perror("fork");
	throw std::string("Could not fork process");
    } else if (m_pid != 0)
    {
	/* parent process */
	close(m_fd[0]);
	m_pout = fdopen(m_fd[1], "w");
	close(m_fd[3]);
	m_pin = fdopen(m_fd[2], "r");
	if (verbose)
	    std::cerr << "Opened stream to child " << m_pid << std::endl;
    } else if (m_pid == 0)
    {
	/* child process */
	close(m_fd[1]);
	dup2(m_fd[0], 0);
	close(m_fd[2]);
	dup2(m_fd[3],1);
	execv(exec_args[0], const_cast<char**>( &exec_args[0] ) );
	close(m_fd[0]);
	throw std::string("Could not execv process");
    }
};

Process::~Process()
{
    int status;
    fclose(m_pout);
    fclose(m_pin);
    for(int i=0; i<4; ++i)
	close(m_fd[i]);
    pid_t pid = wait(&status);
    if (pid < 0)
    {
	perror("wait");
    }
};

std::ostream &operator<<(std::ostream& os, const Process &proc)
{
    
}

void Process::write(const std::string& line)
{
    if (m_pout > 0)
    {
	if (verbose)
	{
	    std::cerr << "writing line to process " << m_pid << std::endl;
	    std::cerr << line;
	    std::cerr << "END LINE" << std::endl;
	}
	//fputs(line.c_str(), m_pout);
	//fputs("\n",m_pout);
	fprintf(m_pout, "%s\n", line.c_str());
	fflush(m_pout);
    }
}

std::string read()
{
    std::string line;
    return line;
}

std::istream &operator>>(std::istream& is, Process &proc)
{
    std::string line;
    is >> line;
    proc.write(line);
    return is;
}
