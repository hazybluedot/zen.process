#ifndef _ZEN_PROCESS_PIPELINE_HPP_
#define _ZEN_PROCESS_PIPELINE_HPP_

#include <sys/types.h>
#include <vector>
#include <string>
#include <memory>

#include "_Process.hpp"

class Pipeline
{
public:
    typedef std::pair<pid_t, int*> value_type;
    typedef ProcessStruct::arg_type arg_type;
    typedef std::vector<arg_type> argv_type;
public:
    Pipeline(const arg_type& argsv, const bool verbose, const std::string& ids="");
    Pipeline(const argv_type& argsv, const bool verbose, const std::string& ids="");
    ~Pipeline();
    
    void write(const std::string& line);
    std::string read();

private:
    bool verbose;
    std::string m_name;
    std::vector<value_type> m_processes;
    int m_fd[2];
    FILE* m_pread;
    FILE* m_pwrite;

    void execute(const argv_type&, const std::string&);
};

typedef Pipeline Process;
#endif
