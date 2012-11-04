#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <signal.h>
#include <sys/wait.h>
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
#include "Pipe.hpp"

#include <boost/filesystem.hpp>
namespace fs = boost::filesystem;

namespace zen {
  namespace process {
    Pipeline::Pipeline(const args_type& argsv, const bool verbose, const opts_type& options) :
      verbose(verbose),
      m_pread((FILE*)NULL),
      m_pwrite((FILE*)NULL)
    {
      argv_type margs(1,argsv);
      argov_type argsov = process_options(margs, options);
      spawn(argsov);
    };

    Pipeline::Pipeline(const argv_type& argsv, const bool verbose, const opts_type& options) :
      verbose(verbose),
      m_pread((FILE*)NULL),
      m_pwrite((FILE*)NULL)
    {
      argov_type argsov = process_options(argsv, options);
      spawn(argsov);
    };

    Pipeline::~Pipeline()
    {
      if (verbose)
	{
	  std::cerr << "Destructor..." << std::endl;
	}

      fclose(m_pwrite);
      fclose(m_pread);

      std::for_each(m_processes.begin(), m_processes.end(), [](value_type p)
		    {
		      kill(p.first, SIGCONT);
		      kill(p.first, SIGTERM);
		      int status(0);
		      pid_t pid(-1);
		      while(-1 == (pid = waitpid(p.first, &status, 0)) && EINTR == errno);
		      if(pid == p.first) {
			std::cerr << "child process " << pid;
			if (WIFEXITED(status)) {
			  std::cerr << " exited with return code " << WEXITSTATUS(status);
			} else if (WIFSIGNALED(status)) {
			  std::cerr << " exited via signal " << WTERMSIG(status) << " (" << strsignal(WTERMSIG(status)) << ")";
			} else {
			  std::cerr << " exited with status " << status;
			}
			std::cerr << std::endl;
		      }
		      else
			std::cerr << "child process " << p.first 
				  << " had already exited" << std::endl;
		  
		    } );
    };

    void Pipeline::spawn(const argov_type& args)
    {
      std::vector<Pipe> pipes;
      const unsigned int numproc = args.size();

      //Generate N+1 pipes for N child processes
      std::generate_n(std::back_inserter(pipes), numproc+1, []() { return Pipe(Pipe::CLOSE_ON_EXEC); });

      for(int pp(0); pp < numproc; ++pp)
	m_processes.push_back(this->exec(args[pp], pipes[pp], pipes[pp+1]));

      if (verbose) {
	std::cerr << "/proc/{" << getpid() << ",";
	std::vector<value_type>::const_iterator it = m_processes.begin();
	std::vector<value_type>::const_iterator end = m_processes.end();
	while (it != end)
	  {
	    std::cerr << it->first;
	    ++it;
	    if (it != end)
	      std::cerr << ",";
	  }
	std::cerr << "}/fd\n";
      }

      int writefd = dup(pipes[0].writefd());
      int readfd = dup(pipes[numproc].readfd());
      pipes[0].closeWrite();
      pipes[numproc].closeRead();

      if (m_pread == (FILE*)NULL) {
	m_pread = fdopen(readfd, "r");
	if (m_pread == (FILE*)NULL)
	  perror("Parent fdopen r");
	if (verbose)
	  std::cerr << "Parent opened read stream on fd" << readfd << std::endl;
      }

      if (m_pwrite == (FILE*)NULL) {
	m_pwrite = fdopen(writefd, "w");
	if (m_pwrite == (FILE*)NULL)
	  perror("Parent fdopen w");
	if (verbose)
	  std::cerr << "Parent opened write stream on fd" << writefd << std::endl;
      }
    };

    Pipeline::value_type Pipeline::exec(const argo_type& pargs, Pipe &in, Pipe &out)
    {
      const args_type args = pargs.first;
      const opts_type opts = pargs.second;
      opts_type::const_iterator oit;
      oit = opts.find("id");
      std::string ids = "";
      if (oit != opts.end())
	{
	  std::stringstream ss;
	  ss << oit->second;
	  ids = ss.str();
	}

      pid_t pid;
      std::string myname = args[0];
  
      SelfPipeTrick spt;
      if ( (pid = fork()) < 0 )
	{
	  std::string what = myname + ": fork failed";
	  return value_type(pid,what);
	} 
      else if ( pid == 0 )
	{
	  /* child */
	  pid_t mypid = getpid();

	  dup2(in.readfd(),0); 
	  dup2(out.writefd(),1);

	  fs::path pathname(args[0]);
	  std::string basename = pathname.filename().native() + ids;
      
	  auto log_stream = [&](std::string basename, std::string key, int fd) 
	    {
	      std::map<std::string,std::string>::const_iterator oit;
	      oit = opts.find("stderr");
	      if (oit != opts.end()) {
		std::string fname = basename + "." + key;
		if (verbose)
		  std::cerr << "Redirecting fd" << fd << " to file " << fname << std::endl;
		int sfd = open(fname.c_str(), O_WRONLY | O_TRUNC | O_CREAT, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
		dup2(sfd,fd); close(sfd);
	      }
	    };

	  log_stream(basename, "stderr", 2);
	  log_stream(basename, "stdlog", 3);

	  spt.execvp(args);
	} 
      else
	{
	  /* parent */
	  in.closeRead();
	  out.closeWrite();
	  
	  try {
	    spt.parent(args);
	  } catch (std::runtime_error e) {
	    std::string what = myname + ": " + e.what(); 
	    return value_type(-pid, what);
	  }
	  if (verbose)
	    std::cerr << "Adding " << args[0] << "[" << pid << "] to parent's [" << getpid() << "] process list" << std::endl;
	  return value_type(pid,myname);		     
	}
    };

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

    std::string Pipeline::read() const
    {
      std::string line;
      char* mystring = NULL;
      size_t num_bytes;
    
      getline(&mystring, &num_bytes, m_pread);
      line = mystring;
      free(mystring);
      return line;
    };

#ifdef HAVE_JSON
    void Pipeline::write(const Json::Value& jvalue)
    {
      std::stringstream oss;
      Json::FastWriter jwriter;
      oss << jwriter.write(jvalue);
      this->write(oss.str());
    };

    Json::Value Pipeline::read_json() const
    {
      Json::Reader jreader;
      Json::Value input;

      std::string line = this->read();
      jreader.parse(line, input);
      return input;
    }
#endif
  }
}
