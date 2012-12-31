/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include <algorithm>
#include <cstdio>
#include <cstdlib>
#include <errno.h>
#include <fcntl.h>
#include <functional>
#include <iostream>
#include <iterator>
#include <signal.h>
#include <sstream>
#include <string.h>
#include <sys/wait.h>
#include <sysexits.h>
#include <unistd.h>

#include "Pipe.hpp"
#include "Pipeline.hpp"
#include "SelfPipeTrickExec.hpp"
#include "utils.hpp"

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
		      get_status(p.first);
		    } );
    };

    void Pipeline::spawn(const argov_type& args)
    {
      std::vector<Pipe> pipes;
      const unsigned int numproc = args.size();

      //Generate N+1 pipes for N child processes
      std::generate_n(std::back_inserter(pipes), numproc+1, []() { return Pipe(Pipe::CLOSE_ON_EXEC); });

      for(unsigned int pp(0); pp < numproc; ++pp)
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

      validate();

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

/**
 * Spawn a new child process and attach its stdin and stdout to pipes.
 * @param pargs = command and arguments to execute child process, e.g. {"/usr/bin/ls", "-a", "-l"}}.
 * @param in = input pipe for new process, will be attached to stdin.
 * @param out = output pipe for new process, will be attached to stdout.
 */
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
  
      SelfPipeTrickExec spt; //Self-pipe trick to report execvp failure to parent
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

	  std::string bname = basename(args[0].c_str());
	  bname += ids;
      
	  auto log_stream = [&](std::string bname, std::string key, int fd) 
	    {
	      std::map<std::string,std::string>::const_iterator oit;
	      oit = opts.find(key);
	      if (oit != opts.end()) {
		std::string fname = bname + "." + key;
		if (verbose)
		  std::cerr << "Redirecting fd" << fd << " to file " << fname << std::endl;
		int sfd = open(fname.c_str(), O_WRONLY | O_TRUNC | O_CREAT, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
		dup2(sfd,fd); close(sfd);
	      }
	    };

	  log_stream(bname, "stderr", 2);
	  log_stream(bname, "stdlog", 3);

	  std::vector<const char*> eav = convert_vs2vc(args);
	  if(-1 == execvp(eav[0], const_cast<char**>(&eav[0])))
	  {
	      spt.writeExecStatus(errno);
	      _exit(0);
	  }

	} 
      else
	{
	  /* parent */
	  in.closeRead();
	  out.closeWrite();
	  
	  int exec_status(spt.checkExecStatus());
	  if(0 != exec_status) { //exec failed
	      std::stringstream ss;
	      ss << myname << ": " << strerror(exec_status) << std::endl;
	      return value_type(-pid, ss.str());
	  } else {
	      return value_type(pid,myname);		     
	  }
	  if (verbose)
	    std::cerr << "Adding " << args[0] << "[" << pid << "] to parent's [" << getpid() << "] process list" << std::endl;
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
        char buffer[1024];
	size_t result = fread(buffer,sizeof(char),1024, m_pread); //TODO: allow user to specify buffer size, do something if there is still data left in buffer after read.
	std::string read_string(buffer);
	return read_string;
    }

    std::string Pipeline::readline() const
    {
      std::string line;
      char* mystring = NULL;
      size_t num_bytes;
    
      getline(&mystring, &num_bytes, m_pread);
      line = mystring;
      free(mystring);
      return line;
    };

#ifdef HAVE_JSONCPP
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

    void Pipeline::validate()
    {
      std::for_each(m_processes.begin(), m_processes.end(), [&](value_type p)
		    {
		      if (p.first > 0) {
			if (verbose)
			  std::cerr << p.second << ": good\n";
		      } else {
			std::cerr << p.second << ": ";
			get_status(-p.first);
		      }
		    } );
    }
  }
}
