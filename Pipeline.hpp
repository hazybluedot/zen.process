#ifndef _ZEN_PROCESS_PIPELINE_HPP_
#define _ZEN_PROCESS_PIPELINE_HPP_

#include <sys/types.h>
#include <vector>
#include <string>

#include "Process.hpp"

class Pipeline
{
public:
    typedef std::pair<pid_t, int*> value_type;
    //typedef std::vector<std::vector<char*> > arg_type;
    typedef std::vector<Process::arg_type> arg_type;
public:
    Pipeline(const arg_type& argsv, const bool verbose);
    ~Pipeline();
    
    void write(const std::string& line);
    std::string read();

private:
    bool verbose;
    std::vector<value_type> m_processes;
    int m_fd[2];
    FILE* m_pread;
    FILE* m_pwrite;

    void execute(const arg_type&);
};

#endif
