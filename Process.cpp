#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>
#include "Process.hpp"

bool is_valid(FILE* fs)
{
    return (fs != (FILE*)NULL);
};

FILE* fdopen_stream(int *fd, const char* direction)
{
    FILE *fp;
    switch(direction[0])
    {
    case 'r':
	close(fd[1]);
	fp = fdopen(fd[0], direction);
	break;
    case 'w':
	close(fd[0]);
	fp = fdopen(fd[1], direction);
	break;
    default:
	throw std::string("Unrecognized file direction");
    }
    if ( fp == (FILE*)NULL )
    {
	perror("fdopen");
	throw std::string("Could not open pipe");
    }
    return fp;
};

Process::Process(const std::vector<char*>& exec_args) : 
    verbose(true), 
    m_pout((FILE*)NULL),
    m_pin((FILE*)NULL),
    m_instring(NULL)
{
    int nbytes = 100;
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
    
    /* watch input  */
    m_fds[0].fd = m_fd[2];
    m_fds[0].events = ( POLLIN | POLLHUP | POLLERR );

    if (verbose)
	std::cerr << "Forking..." << std::endl;
    m_pid = fork();

    if (m_pid < 0)
    {
	perror("fork");
	throw std::string("Could not fork process");
    } else if (m_pid != 0)
    {
	/* parent process */
	m_pout = fdopen_stream(m_fd, "w");
	m_pin = fdopen_stream(m_fd+2, "r");
	if (verbose)
	    std::cerr << "Opened stream to child " << m_pid << " with output stream " << m_pout << " and input stream " << m_pin << std::endl;
    } else if (m_pid == 0)
    {
	/* child process */
	close(m_fd[1]);
	dup2(m_fd[0], 0);
	close(m_fd[2]);
	dup2(m_fd[3],1);
	execvp(exec_args[0], const_cast<char**>( &exec_args[0] ) );
	close(m_fd[0]);
	throw std::string("Could not execv process");
    }
};

Process::Process(Process&& other) : 
    verbose(false), 
    m_pout((FILE*)NULL),
    m_pin((FILE*)NULL),
    m_instring(NULL)
{
    verbose = other.verbose;
    m_pout = other.m_pout;
    m_pin = other.m_pin;
    m_pid = other.m_pid;
    m_instring = other.m_instring;

    other.m_pout = (FILE*)NULL;
    other.m_pin = (FILE*)NULL;
    other.m_pid = (pid_t)NULL;
}

Process::~Process()
{
    std::cerr << "Calling ~Process()" << std::endl;
    int status;
    if (m_pout != (FILE*)NULL)
	fclose(m_pout);
    if (m_pout != (FILE*)NULL)
	fclose(m_pin);
    free(m_instring);
    for(int i=0; i<4; ++i)
	close(m_fd[i]);
    pid_t pid = ::wait(&status);
    if (pid < 0)
    {
	perror("wait");
    }
};

std::ostream &operator<<(std::ostream& os, const Process &proc)
{
    return os;
}

void Process::write(const std::string& line)
{
//    if (m_pout != (FILE*)NULL)
    if (is_valid(m_pout))
    {
	if (verbose)
	{
	    std::cerr << "writing line to process " << m_pid << std::endl;
	    std::cerr << line;
	}
	fputs(line.c_str(), m_pout);
	fflush(m_pout);
    }
}
void Process::close_output()
{
    if (is_valid(m_pout))
    {
	int ret = fclose(m_pout);
	if (ret == 0)
	    m_pout = (FILE*)NULL;
    }
}

std::string Process::read()
{
    std::string line;
    char* mystring = NULL;
    size_t num_bytes;

    if (feof(m_pin))
    {
	std::cerr << "Process closed file descriptor" << std::endl;
	return line;
    };

    int ret  = poll(m_fds,1,1000);
    if ( ret == -1 )
    {
	throw std::string("poll");
    }
    if (!ret)
    { /* timeout occured */
	return line;
    }
    
    if (m_fds[0].revents & POLLHUP)
    {
	std::cerr << "File descriptor hung up" << std::endl;
    }

    if (m_fds[0].revents & POLLIN)
    {
	getline(&mystring, &num_bytes, m_pin);
	line = mystring;
    }
    return line;
}

int Process::wait()
{
    pid_t pid = ::wait(&m_status);
    if (pid == m_pid)
	return m_status;
    else
	return -1;
}

std::istream &operator>>(std::istream& is, Process &proc)
{
    std::string line;
    is >> line;
    proc.write(line);
    return is;
}
