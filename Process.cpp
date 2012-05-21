#include <fcntl.h>
#include <unistd.h>
#include <string.h>

#include <cstdlib>
#include <cstdio>
#include <sys/wait.h>
#include <algorithm>
#include "utils.hpp"
#include "Process.hpp"
#include "selfpipetrick.hpp"

#include <boost/filesystem.hpp>
namespace fs = boost::filesystem;

bool is_valid(FILE* fs)
{
    return (fs != (FILE*)NULL);
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

Process::Process(const arg_type& exec_args, bool verbose,  const std::string& ids) : 
    verbose(verbose), 
    m_name(exec_args[0]),
    m_pid((pid_t)NULL),
    m_writepipe {-1,-1},
    m_readpipe {-1,-1},
    m_pout((FILE*)NULL),
    m_pin((FILE*)NULL),
    m_instring(NULL)
{
    create(exec_args, ids);
}

void Process::create(const arg_type& exec_args, const std::string& ids)
{

    if ( pipe(m_readpipe) < 0  ||  pipe(m_writepipe) < 0 )
    {
	/* FATAL: cannot create pipe */
	/* close readpipe[0] & [1] if necessary */
	perror("pipe");
	throw std::string("Could not create pipe");
    }

    /* watch input  */
    //m_fds[0].fd = PARENT_READ;
    //m_fds[0].events = ( POLLIN | POLLHUP | POLLERR );

    if (verbose)
	std::cerr << "Process " << m_name << ": Forking..." << std::endl;
    
    SelfPipeTrick spt;

    if ( (m_pid = fork()) < 0)
    {
	perror("fork");
	throw std::string("Could not fork process");
    } else if (m_pid != 0)
    {
	/* parent process */
	//TODO: handle SIGCHLD to clean up Zombie children
	close(CHILD_READ);
	close(CHILD_WRITE);

	try {
	    spt.parent(exec_args);
	} catch (std::runtime_error e)
	{
	    std::cerr << "Error: " << e.what() << std::endl;
	    throw e;
	}
	m_pout = fdopen(PARENT_WRITE, "w");
	if (m_pout == NULL)
	    perror("Process fdopen(PARENT_WRITE)");
	m_pin = fdopen(PARENT_READ, "r");	
	if (m_pin == NULL)
	    perror("Process fdopen(PARENT_READ)");
	if (verbose)
	    std::cerr << "Process " << m_name << ": Opened stream to child " << m_pid << " with output stream " << m_pout << " and input stream " << m_pin << std::endl;

    } else if (m_pid == 0)
    {
	/* child process */
	close(PARENT_WRITE);
	close(PARENT_READ);
	dup2(CHILD_READ, 0); close(CHILD_READ);
	dup2(CHILD_WRITE,1); close(CHILD_WRITE);

	fs::path pathname(m_name);
	std::string basename = pathname.filename().native();
	std::string error_log = basename + ids + ".error";
	if (verbose)
	    std::cerr << "Redirecting standard error to file " << error_log << std::endl;
	mode_t mode = S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH;
	int fd = open(error_log.c_str(), O_WRONLY | O_TRUNC | O_CREAT, mode);
	dup2(fd,2); close(fd);

	spt.execvp(exec_args);
    }
};

Process::~Process()
{
    if (verbose)
    {
	std::cerr << "Calling ~Process() for " << this->m_name << std::endl;
	std::cerr << *this;
    }

    int status;

    if (m_pout != NULL)
	if (fclose(m_pout) != 0)
	    perror("fclose m_pout");

    if (m_pid != 0)
    {
	if (verbose)
	    std::cerr << "Calling wait on PID " << m_pid << std::endl;
	//kill(m_pid, SIGTERM);
        while(::waitpid(m_pid, &status, 0) == -1)
	    if (errno != EINTR) {
		perror("waitpid");
	    }
    }

    if (m_pin != NULL)
	if (fclose(m_pin) != 0)
	    perror("fclose m_pin");


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
    if (is_valid(m_pout))
    {
	if (verbose)
	{
	    std::cerr << "writing line to process " << m_pid << std::endl;
	    std::cerr << line;
	}
	fputs(line.c_str(), m_pout);
	if (ferror(m_pout))
	    std::cerr << "Error writing to process " << m_pid << std::endl; 
	fflush(m_pout);
    }
}
void Process::close_output()
{
    if (m_pout != NULL)
    {
	if (fclose(m_pout) != 0)
	    perror("fclose m_pout");
	else
	    m_pout = NULL;
    }
}

std::string Process::read()
{
    std::string line;
    char* mystring = NULL;
    size_t num_bytes;

    getline(&mystring, &num_bytes, m_pin);
    line = mystring;
    free(mystring);
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

