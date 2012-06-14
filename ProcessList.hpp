#ifndef _ZEN_PROCESS_LIST_HPP_
#define _ZEN_PROCESS_LIST_HPP_

#include <vector>
#include <memory>

#include "Pipeline.hpp"

class ProcessList
{
public:
    typedef std::unique_ptr<Process> value_type;
    typedef std::vector<value_type>::iterator iterator;
    typedef std::vector<value_type>::const_iterator const_iterator;

    ProcessList(int N,  const Process::arg_type&, bool verbose);
    ~ProcessList();

    inline iterator begin() { return m_ProcessList.begin(); };
    inline iterator end() { return m_ProcessList.end(); };

private:
    std::vector<std::unique_ptr<Process> > m_ProcessList;

    void sigchld_handler(int signo);
};

#endif
