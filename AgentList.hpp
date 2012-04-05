#ifndef _ZEN_AGENTLIST_HPP_
#define _ZEN_AGENTLIST_HPP_

#include <sys/types.h>
#include <map>
#include <vector>

class AgentList
{
/**
   Spawn N identicle processes specified by agent_args
   Manage file descriptors to each one, provide an iterator to output filedescriptors
   Deconstructor waits for children to execute.  
**/
public:
    typedef std::pair<pid_t,int> value_type;
    typedef std::map<pid_t,int>::iterator iterator;
    typedef std::map<pid_t,int>::const_iterator const_iterator;

public:
    AgentList(size_t N, const std::vector<char*>& args, bool verbose=false);
    ~AgentList();

    iterator begin() { return m_pids.begin(); };
    //const_iterator begin() { return m_pids.begin(); };
    iterator end() { return m_pids.end(); };
    //const_iterator end() { return m_pids.end(); };

private:
    bool verbose;
    void execute(const std::vector<char*>&);
    void close_all();
    std::map<pid_t,int> m_pids;
};

#endif
