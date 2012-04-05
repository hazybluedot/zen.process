#ifndef _ZEN_PROCESS_HPP_
#define _ZEN_PROCESS_HPP_

#include <sys/types.h>
#include <iostream>
#include <vector>

class Process
{
public:
    Process(const std::vector<char*>&);
    ~Process();

    friend std::ostream &operator<<(std::ostream& os, const Process &proc);
    friend std::istream &operator>>(std::istream &is, Process &proc);

private:
    pid_t m_pid;
    //int m_fd;
    FILE* m_pout;
    FILE* m_pin;
};

#endif
