#ifndef _ZEN_PROCESS_PIPELINE_HPP_
#define _ZEN_PROCESS_PIPELINE_HPP_

#include <sys/types.h>
#include <vector>
#include <string>
#include <memory>

#include "_Process.hpp"
#include <jsoncpp/json.h>

class Pipeline
{
public:
    typedef std::pair<pid_t, int*> value_type;
    typedef ProcessStruct::arg_type arg_type;
    typedef std::vector<arg_type> argv_type;
    typedef std::map<std::string,std::string> opts_type;
    typedef opts_type options;
public:
    Pipeline(const arg_type& argsv, const bool verbose, const opts_type& options = opts_type());
    Pipeline(const argv_type& argsv, const bool verbose, const opts_type& options = opts_type());
    ~Pipeline();
    
    void write(const std::string& line);
    std::string read() const;
    
    void write(const Json::Value& jvalue);
    Json::Value read_json() const;

private:
    bool verbose;
    std::string m_name;
    std::vector<value_type> m_processes;
    int m_fd[2];
    FILE* m_pread;
    FILE* m_pwrite;

    void execute(const argv_type&, const opts_type&);
};

typedef Pipeline Process;
#endif
