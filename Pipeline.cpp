#include <fcntl.h>
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

#include "utils.hpp"
#include "Pipeline.hpp"

Pipeline::Pipeline(const arg_type& argsv, const bool verbose) :
    verbose(verbose)
{
    execute(argsv);
};

Pipeline::~Pipeline()
{
    fclose(m_pwrite);
    if (verbose)
	std::cerr << "Closed write stream" << std::endl;
    for_each(m_processes.begin(), m_processes.end(), [](value_type p)
	     {
		 //std::cerr << "Closing file descriptors for PID " << p.first << std::endl;
		 close(p.second[0]);
		 close(p.second[1]);
		 free(p.second);
	     }
	);
    for_each(m_processes.begin(), m_processes.end(), [](value_type p)
	     {
		 int status;

		 while(waitpid(p.first,&status,0) == -1)
		     if (errno != EINTR) {
			 perror("~Pipeline waitpid");
		     }
	     }
	);
    close(m_fd[0]);
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
    for_each(argsv.begin(),argsv.end(), [this,end,begin,&it](std::vector<std::string> args)
	     {
		 pid_t pid;
		 int pipefds[2];
		 int count, err;

		 /* self-pipe trick: http://cr.yp.to/docs/selfpipe.html
		    implementation example: http://stackoverflow.com/questions/1584956/how-to-handle-execvp-errors-after-fork */
		 if (pipe(pipefds)) {
		     perror("pipe");
		     return EX_OSERR;
		 }
		 if (fcntl(pipefds[1], F_SETFD, fcntl(pipefds[1], F_GETFD) | FD_CLOEXEC)) {
		     perror("fcntl");
		     return EX_OSERR;
		 }
		 /* end self-pipe trick */

		 if ( (pid = fork()) < 0 )
		 {
		     perror("fork");
		     throw std::string("Error forking");
		 } 
		 else if ( pid == 0 )
		 {
		     /* child */
		     close(it->second[1]);
		     dup2(it->second[0],0); close(it->second[0]);
		     if (it+1 != end)
		     {
			 dup2((it+1)->second[1],1); close((it+1)->second[1]);
			 close(m_fd[1]);
		     } else {
			 dup2(m_fd[1],1); close(m_fd[1]);
		     }
		     for_each(m_processes.begin(), m_processes.end(),  [](value_type p)
			      {
				  close(p.second[0]);
				  close(p.second[1]);
			      } );
		     close(pipefds[0]); //self-pipe trick
		     close(m_fd[0]);
		     std::vector<const char*> vc = convert_vs2vc(args);
		     execvp(vc[0], const_cast<char**>( &vc[0] ) );
		     ::write(pipefds[1], &errno, sizeof(int)); //self-pipe trick
		     _exit(0);
		     /*
		     std::string errormsg = "child [";
		     errormsg.append(vc[0]);
		     errormsg.append("] execvp");
		     perror(errormsg.c_str());
		     */
		 } 
		 else
		 {
		     /* parent */
		     close(it->second[0]);
		     if (it!=begin)
			 close(it->second[1]);
		     if (verbose)
			 std::cerr << "Adding " << pid << " to parent's process list" << std::endl;

		     /* self-pipe trick parent side */
		     close(pipefds[1]);
		     while ((count = ::read(pipefds[0], &err, sizeof(errno))) == -1)
			 if (errno != EAGAIN && errno != EINTR) break;
		     if (count) {
			 fprintf(stderr, "child's execvp: %s\n", strerror(err));
			 return EX_UNAVAILABLE;
		     }
		     close(pipefds[0]);
		     /* end self-pipe trick parent side */
		     it->first = pid; //TODO: check that this really is a reference and PIDs are saved in parent
		 
		 }
		 ++it;
	     }
	);
    close(m_fd[1]);
    if (verbose)
	std::cerr << "Parent opening read stream on fd" << m_fd[0] << std::endl;
    m_pread = fdopen(m_fd[0], "r");
    if (m_pread == (FILE*)NULL)
	perror("Parent fdopen r");
    if (verbose)
	std::cerr << "Parent opening write stream on fd" << m_processes.begin()->second[1] << std::endl;
    m_pwrite = fdopen(m_processes.begin()->second[1], "w");
    if (m_pwrite == (FILE*)NULL)
	perror("Parent fdopen w");
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
