#include <fcntl.h>
#include <unistd.h>

#include <cstdlib>
#include <cstdio>
#include <sys/wait.h>
#include <algorithm>
#include "utils.hpp"
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

Process::Process(const ProcessStruct& pstruct) :
    verbose(pstruct.verbose), 
    m_name(pstruct.args[0]),
    m_pid((pid_t)NULL),
    m_writepipe {-1,-1},
    m_readpipe {-1,-1},
    m_pout((FILE*)NULL),
    m_pin((FILE*)NULL),
    m_instring(NULL)
{
    create(pstruct.args);
}

Process::Process(const arg_type& exec_args, bool verbose) : 
    verbose(verbose), 
    m_name(exec_args[0]),
    m_pid((pid_t)NULL),
    m_writepipe {-1,-1},
    m_readpipe {-1,-1},
    m_pout((FILE*)NULL),
    m_pin((FILE*)NULL),
    m_instring(NULL)
{
    create(exec_args);
}

void Process::create(const arg_type& exec_args, const std::string& stderr)
{

    if ( pipe(m_readpipe) < 0  ||  pipe(m_writepipe) < 0 )
    {
	/* FATAL: cannot create pipe */
	/* close readpipe[0] & [1] if necessary */
	perror("pipe");
	throw std::string("Could not create pipe");
    }

    /* watch input  */
    m_fds[0].fd = PARENT_READ;
    m_fds[0].events = ( POLLIN | POLLHUP | POLLERR );

    if (verbose)
	std::cerr << "Process " << m_name << ": Forking..." << std::endl;

    if ( (m_pid = fork()) < 0)
    {
	perror("fork");
	throw std::string("Could not fork process");
    } else if (m_pid != 0)
    {
	/* parent process */
	close(CHILD_READ);
	close(CHILD_WRITE);
	m_pout = fdopen(PARENT_WRITE, "w");
	m_pin = fdopen(PARENT_READ, "r");
	if (verbose)
	    std::cerr << "Process " << m_name << ": Opened stream to child " << m_pid << " with output stream " << m_pout << " and input stream " << m_pin << std::endl;
    } else if (m_pid == 0)
    {
	/* child process */
	close(PARENT_WRITE);
	close(PARENT_READ);
	dup2(CHILD_READ, 0); //close(CHILD_READ);
	dup2(CHILD_WRITE,1); //close(CHILD_WRITE);
	//if (stderr != "")
	//{
	std::string error_log = m_name + ".error";
	if (verbose)
	    std::cerr << "Redirecting standard error to file " << error_log << std::endl;
	int fd = open(error_log.c_str(), O_WRONLY | O_CREAT);
	dup2(fd,2);
	    //}
	//std::vector<const char*> vc = convert_vs2vc(exec_args);
	std::vector<const char*> vc;
	std::transform(exec_args.begin(), exec_args.end(), std::back_inserter(vc), [](const std::string& s) {
		return s.c_str();
	    }
	    );

	execvp(vc[0], const_cast<char**>( &vc[0] ) );
	perror("execvp");
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
    if (verbose)
    {
	std::cerr << "Calling ~Process() for " << this->m_name << std::endl;
	std::cerr << *this;
    }

    int status;
    close_stream(m_pout);
    close_stream(m_pin);
    free(m_instring);

    /*
    for(int i=0; i<4; ++i)
	close(m_fd[i]);
    */

    if (m_pid != 0)
    {
	if (verbose)
	    std::cerr << "Calling wait on PID " << m_pid << std::endl;
	pid_t pid = ::waitpid(m_pid, &status, 0);
	if (pid < 0)
	{
	    perror("wait");
	}
    }
    if (verbose)
    {
	std::cerr << "Process " << m_name << ": leaving ~Process()" << std::endl;
	std::cerr << *this;
    }
};

std::ostream &operator<<(std::ostream& os, const Process &proc)
{
    os << "Process " << proc.m_name << std::endl;
    os << "\tPID: " << proc.m_pid <<  std::endl;
    os << "\tInput Stream: " << proc.m_pin << std::endl;
    os << "\tOutput Stream: " << proc.m_pout << std::endl;
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
    close_stream(m_pout);
}

std::string Process::read()
{
    std::string line;
    char* mystring = NULL;
    size_t num_bytes;

    getline(&mystring, &num_bytes, m_pin);
    line = mystring;
    return line;
}

/*
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
    { // timeout occured 
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
*/

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

int Process::close_stream(FILE* stream)
{
    int ret=0;
    if (stream != (FILE*)NULL && is_valid(stream) && ferror(stream) == 0)
    {
	if (verbose)
	    std::cerr << "Closing stream " << stream << ": ";
	int fd = fileno(stream);
	int ret = fclose(stream);
	if (ret == 0)
	    stream = (FILE*)NULL;
	if (verbose)
	    std::cerr << ret << " ";
	ret = close(fd);
	if (verbose)
	    std::cerr << ret << std::endl;
    }
    return ret;
}
