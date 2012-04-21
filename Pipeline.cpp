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
    verbose(verbose)
{
    execute(argsv);
};

Pipeline::~Pipeline()
{
    if (verbose)
    {
	std::cerr << "Destructor..." <<std::endl;
	sleep(1);
    }
    if (verbose)
	std::cerr << "Closed fd" << fileno(m_pwrite) << ", parent write stream";
    fclose(m_pwrite);
    pid_t firstpid = m_processes.front().first;
    for_each(m_processes.begin(), m_processes.end(), [firstpid,this](value_type p)
	     {
		 //std::cerr << "Closing file descriptors for PID " << p.first << std::endl;
		 int status;
		 if (p.first != firstpid)
		 {
		     close(p.second[1]);
		     if (verbose)
			 std::cerr << "Closed fd" << p.second[1];
		 }
		 if (verbose)
		 {
		     sleep(1);
		     std::cerr << ", waiting for " << p.first << "...";
		 }
		 while(waitpid(p.first,&status,0) == -1)
		     if (errno != EINTR) {
			 perror("~Pipeline waitpid");
		     }
		 if (verbose)
		 {
		     sleep(1);
		     std::cerr << " done with status " << status << std::endl;
		 }
	     }
	);
    for_each(m_processes.begin(), m_processes.end(), [this](value_type p)
	     {
		 if (verbose)
		     std::cerr << "Closed fd" << p.second[0] << std::endl;
		 close(p.second[0]);
		 free(p.second);
	     }
	);
    //if (verbose)
//	std::cerr << "Closed fd" << m_fd[0] << std::endl;
    //close(m_fd[0]);
    if (verbose)
	std::cerr << "Closed fd" << fileno(m_pread) << ", parent read stream" <<std::endl;
    fclose(m_pread);
};

void Pipeline::execute(const arg_type& argsv)
{
    pipe(m_fd);
    if (verbose)
	std::cerr << "Created pipe " << m_fd[1] << "=>" << m_fd[0] << std::endl;
    for_each(argsv.begin(),argsv.end(), [this](Process::arg_type args)
	     {
		 pid_t pid = (pid_t)0;
		 int* fd = (int*)calloc(2,sizeof(int));
		 pipe(fd);
		 if (verbose)
		     std::cerr << "Created pipe " << fd[1] << "=>" << fd[0] << std::endl;
		 m_processes.push_back(value_type(pid,fd));
	     }
	);

    std::vector<value_type>::const_iterator begin = m_processes.begin();
    std::vector<value_type>::iterator it = m_processes.begin();
    std::vector<value_type>::const_iterator end = m_processes.end();

    std::vector<int> my_fds;
    my_fds.push_back(m_fd[0]);
    my_fds.push_back(m_fd[1]);
    std::vector<int>::iterator pend = my_fds.end();

    for_each(argsv.begin(),argsv.end(), [this,end,begin,&it,&my_fds,&pend](std::vector<std::string> args)
	     {
		 pid_t pid;
		 pid_t mypid;
		 std::string myname("parent");

		 SelfPipeTrick spt;

		 std::function<void (const int)> close = [&,this](const int fd)
		     {
			 std::vector<int>::const_iterator oldend = pend;
			 pend = std::remove_if(my_fds.begin(),pend, [fd](int p) { return (p == fd); } );
			 if (pend != oldend)
			 {
			     if (verbose)
				 std::cerr << myname << "[" << mypid << "] closing fd " << fd << ". " << pend-my_fds.begin() << " left." << std::endl;
			     ::close(fd);
			 }
		     };

		 sleep(1);
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
		     for_each(m_processes.begin(), m_processes.end(),  [&my_fds](value_type p)
			      {
				  my_fds.push_back(p.second[0]);
				  my_fds.push_back(p.second[1]);
			      } );
		     pend = my_fds.end();
		     if (verbose)
		     {
			 std::cerr << args[0] << "[" << mypid << "] has fds ";
			 std::copy(my_fds.begin(),pend, std::ostream_iterator<int>(std::cerr, ", "));
			 std::cerr << std::endl;
		     }

		     if (verbose)
			 std::cerr << "fd" << it->second[0] << "=>" << args[0] << "[" << mypid << "]=>";
		     close(it->second[1]);
		     dup2(it->second[0],0); close(it->second[0]);
		     if (it+1 != end)
		     {
			 dup2((it+1)->second[1],1); close((it+1)->second[1]);
		     } else {
			 dup2(m_fd[1],1);
		     }
		     close(m_fd[1]);

		     if (verbose)
		     {
			 std::cerr << "closing remaing fds: ";
			 std::copy(my_fds.begin(), pend, std::ostream_iterator<int>(std::cerr, ", "));
			 std::cerr << std::endl;
		     }
		     std::for_each(my_fds.begin(), pend, [](int fd) { ::close(fd); } );
		     //spt.execvp(args);
		     std::vector<const char*> vc = convert_vs2vc(args);
		     ::execvp(vc[0], const_cast<char**>( &vc[0] ) );
		     std::cerr << "Blah." << std::endl;

		 } 
		 else
		 {
		     /* parent */

		     int fd = m_fd[0];
		     if (m_pread == (FILE*)NULL)
		     {
			 m_pread = fdopen(fd, "r");
			 if (m_pread == (FILE*)NULL)
			     perror("Parent fdopen r");
			 if (verbose)
			     std::cerr << "Parent opened read stream on fd" << fd << std::endl;
			 pend = std::remove_if(my_fds.begin(),pend, [fd](int p) { return (p == fd); } );
		     }

		     fd = m_processes.begin()->second[1];
		     if (m_pwrite == (FILE*)NULL)
		     {
			 m_pwrite = fdopen(fd, "w");
			 if (m_pwrite == (FILE*)NULL)
			     perror("Parent fdopen w");
			 if (verbose)
			     std::cerr << "Parent opened write stream on fd" << fd << std::endl;
			 pend = std::remove_if(my_fds.begin(),pend, [fd](int p) { return (p == fd); } );
		     }

		     mypid = getpid();
		     for_each(m_processes.begin(), m_processes.end(),  [&my_fds](value_type p)
			      {
				  my_fds.push_back(p.second[0]);
				  my_fds.push_back(p.second[1]);
			      } );
		     pend = my_fds.end();

		     close(it->second[0]);
		     if (it!=begin)
			 close(it->second[1]);

		     close(m_fd[1]);
		     
		     /*
		       try 
		       { 
		       spt.parent(args);
		       } catch (std::runtime_error e)
		       {
		       //clean up stuff
		       std::cerr << "Exec error: " << e.what() << std::endl;
		       throw e;
		       }
		     */
		     if (verbose)
			 std::cerr << "Adding " << args[0] << "[" << pid << "] to parent's process list" << std::endl;
		     it->first = pid; //TODO: check that this really is a reference and PIDs are saved in parent

		 }
		 ++it;
	     }
	);

    if (verbose)
    {
	std::cerr << "parent has remaing fds: ";
	std::copy(my_fds.begin(), pend, std::ostream_iterator<int>(std::cerr, ", "));
	std::cerr << std::endl;
    }
    //std::for_each(my_fds.begin(), pend, [](int fd) { ::close(fd); } );
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
