#include <fcntl.h>
#include <unistd.h>

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
    for_each(m_processes.begin(), m_processes.end(), [](value_type p)
	     {
		 close(p.second[0]);
		 close(p.second[1]);
		 free(p.second);
	     }
	);
    for_each(m_processes.begin(), m_processes.end(), [](value_type p)
	     {
		 int status;
		 waitpid(p.first,&status,0);
	     }
	);
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
		 if ( (pid = fork()) < 0 )
		 {
		     perror("fork");
		     throw std::string("Error forking");
		 } 
		 else if ( pid == 0 )
		 {
		     /* child */
		     std::vector<value_type>::iterator cit = m_processes.begin();
		     while (cit != it)
		     {
			 close(cit->second[0]);
			 close(cit->second[1]);
			 ++cit;
		     }
		     close(it->second[1]);
		     dup2(it->second[0],0); //close(0);
		     if (it+1 != end)
		     {
			 dup2((it+1)->second[1],1); //close(1);
			 close(m_fd[1]);
		     } else {
			 dup2(m_fd[1],1); //close(1);
		     }
		     close(m_fd[0]);
		     std::vector<const char*> vc = convert_vs2vc(args);
		     execvp(vc[0], const_cast<char**>( &vc[0] ) );
		     perror("child execvp");
		     exit(1);
		 } 
		 else
		 {
		     /* parent */
		     close(it->second[0]);
		     if (it!=begin)
			 close(it->second[1]);
		     if (verbose)
			 std::cerr << "Adding " << pid << " to parent's process list" << std::endl;
		     it->first = pid; //TODO: check that this really is a reference and PIDs are saved in parent
		 }
		 ++it;
	     }
	);
    close(m_fd[1]);
    if (verbose)
	std::cerr << "Parent opening read stream on fd" << m_fd[0] << std::endl;
    m_pread = fdopen(m_fd[0], "r");
    if (verbose)
	std::cerr << "Parent opening write stream on fd" << m_processes.begin()->second[1] << std::endl;
    m_pwrite = fdopen(m_processes.begin()->second[1], "w");
}

void Pipeline::write(const std::string& line)
{
    if (verbose)
    {
	std::cerr << "writing line to process " << m_processes.at(0).first << std::endl;
	std::cerr << line;
    }
    fputs(line.c_str(), m_pwrite);
    fflush(m_pwrite);
};

std::string Pipeline::read()
{
    std::string line;
    char* mystring = NULL;
    size_t num_bytes;
    
    getline(&mystring, &num_bytes, m_pread);
    line = mystring;
    return line;
};
