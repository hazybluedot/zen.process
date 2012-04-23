#include <unistd.h>
#include <errno.h>
#include <sysexits.h>
#include <string.h>

#include <cstdlib>
#include <cstdio>
#include <sys/wait.h>
#include <algorithm>
#include <iostream>
#include <sstream>
#include <functional>
#include <iterator>

#include "utils.hpp"
#include "selfpipetrick.hpp"
#include "Pipeline.hpp"

Pipeline::Pipeline(const arg_type& argsv, const bool verbose) :
    verbose(verbose),
    m_fd {-1,-1},
    m_pread((FILE*)NULL),
    m_pwrite((FILE*)NULL)
{
    execute(argsv);
};

Pipeline::~Pipeline()
{
    if (verbose)
    {
	std::cerr << "Destructor..." <<std::endl;
    }
    if (verbose)
	std::cerr << "Closed fd" << fileno(m_pwrite) << ", parent write stream";
    fclose(m_pwrite);

    typedef std::vector<value_type>::iterator iter_val;
    std::reverse_iterator<iter_val> rev_end (m_processes.begin());
    std::reverse_iterator<iter_val> rev_begin (m_processes.end());

    pid_t firstpid = m_processes.back().first;
    for_each(rev_begin, rev_end, [firstpid,this](value_type p)
	     {
		 //std::cerr << "Closing file descriptors for PID " << p.first << std::endl;
		 int status;

		 if (verbose)
		 {
		     std::cerr << ", waiting for " << p.first << "...";
		 }
		 kill(p.first, SIGTERM);
		 while(waitpid(p.first,&status,0) == -1)
		     if (errno != EINTR) {
			 perror("~Pipeline waitpid");
		     }
		 if (verbose)
		 {
		     std::cerr << " done with status " << status;
		 }
	     }
	);

    for_each(m_processes.begin(), m_processes.end(), [this](value_type p)
	     {
		 free(p.second);
	     }
	);

    if (verbose)
	std::cerr << "Closed fd" << fileno(m_pread) << ", parent read stream" <<std::endl;
    fclose(m_pread);
};

void Pipeline::execute(const arg_type& argsv)
{
    pipe(m_fd);
 
    std::vector<value_type>::const_iterator begin = m_processes.begin();
    std::vector<value_type>::iterator it = m_processes.begin();
    std::vector<value_type>::const_iterator end = m_processes.end();

    std::vector<int> my_fds;
    my_fds.push_back(m_fd[0]);
    my_fds.push_back(m_fd[1]);
    std::vector<int>::iterator pend = my_fds.end();

    typedef arg_type::const_iterator iter_argsv;
    std::reverse_iterator<iter_argsv> rev_end (argsv.begin());
    std::reverse_iterator<iter_argsv> rev_begin (argsv.end());
    
    int *this_out;
    int *next_out = &m_fd[1];
    for_each(rev_begin, rev_end, [this,end,begin,&it,&my_fds,&pend,&this_out,&next_out](std::vector<std::string> args)
	     {
		 pid_t pid;
		 pid_t mypid;
		 std::string myname("parent");
		 
		 int* fd = (int*)calloc(2,sizeof(int));
		 pipe(fd);

		 this_out = next_out;
		 next_out = &fd[1];

		 if (verbose)
		     std::cerr << "Created pipe " << fd[1] << "=>" << fd[0] << std::endl;
		 
		 SelfPipeTrick spt;

		 std::function<void (const int)> close = [&,this](const int fd)
		     {
			 std::vector<int>::const_iterator oldend = pend;
			 pend = std::remove_if(my_fds.begin(),pend, [fd](int p) { return (p == fd); } );
			 if (pend != oldend)
			 {
			     //if (verbose)
			     // std::cerr << myname << "[" << mypid << "] closing fd " << fd << ". " << pend-my_fds.begin() << " left." << std::endl;
			     ::close(fd);
			 }
		     };

		 my_fds.push_back(fd[0]);
		 my_fds.push_back(fd[1]);

		 if ( (pid = fork()) < 0 )
		 {
		     perror("fork");
		     throw std::string("Error forking");
		 } 
		 else if ( pid == 0 )
		 {
		     /* child */
		     mypid = getpid();
		     myname = args[0];

		     pend = my_fds.end();

		     if (verbose)
			 std::cerr << "fd" << fd[0] << "=>" << args[0] << "[" << mypid << "]=>fd" << *this_out << std::endl;
		     close(fd[1]);
		     dup2(fd[0],0); close(fd[0]);
		     dup2(*this_out,1); close(*this_out);

		     if (verbose)
		     {
			 std::cerr << "closing remaing fds: ";
			 std::copy(my_fds.begin(), pend, std::ostream_iterator<int>(std::cerr, ", "));
			 std::cerr << std::endl;
		     }
		     std::for_each(my_fds.begin(), pend, [](int fd) { ::close(fd); } );
		     spt.execvp(args);
		 } 
		 else
		 {
		     /* parent */
		     if (verbose)
			 std::cerr << "Adding " << args[0] << "[" << pid << "] to parent's process list" << std::endl;
		     m_processes.push_back(value_type(pid,fd));

		     try {
			 spt.parent(args);
		     } catch (std::runtime_error e) {
			 pend = my_fds.end();
			 close(m_fd[1]);
			 std::for_each(my_fds.begin(), pend, [](int fd) { ::close(fd); } );
			 std::for_each(m_processes.begin(), m_processes.end(), [this](value_type p) { free(p.second); } );
			 throw e;
		     }
		     
		 }
		 ++it;
	     }
	);

    close(m_fd[1]);

    pend = my_fds.end();
    int rfd = m_fd[0];
    if (m_pread == (FILE*)NULL)
    {
	m_pread = fdopen(rfd, "r");
	if (m_pread == (FILE*)NULL)
	    perror("Parent fdopen r");
	if (verbose)
	    std::cerr << "Parent opened read stream on fd" << rfd << std::endl;
	pend = std::remove_if(my_fds.begin(),pend, [rfd](int p) { return (p == rfd); } );
    }

    int wfd = *next_out;
    if (m_pwrite == (FILE*)NULL)
    {
	m_pwrite = fdopen(wfd, "w");
	if (m_pwrite == (FILE*)NULL)
	    perror("Parent fdopen w");
	if (verbose)
	    std::cerr << "Parent opened write stream on fd" << wfd << std::endl;
	pend = std::remove_if(my_fds.begin(),pend, [wfd](int p) { return (p == wfd); } );
    }

    if (verbose)
    {
	std::cerr << "parent has remaing fds: ";
	std::copy(my_fds.begin(), pend, std::ostream_iterator<int>(std::cerr, ", "));
	std::cerr << std::endl;
    }
    std::for_each(my_fds.begin(), pend, [](int fd) { ::close(fd); } );
}

void Pipeline::write(const std::string& line)
{
    if (verbose)
    {
	std::cerr << "writing line to process " << m_processes.at(0).first << std::endl;
	std::cerr << line;
    }
    if (m_pwrite != (FILE*)NULL)
    {
	fputs(line.c_str(), m_pwrite);
	fflush(m_pwrite);
    }
};

std::string Pipeline::read()
{
    std::string line;
    char* mystring = NULL;
    size_t num_bytes;
    
    getline(&mystring, &num_bytes, m_pread);
    line = mystring;
    free(mystring);
    return line;
};
