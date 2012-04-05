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

    void write(const std::string&);
    std::string read();

    friend std::ostream &operator<<(std::ostream& os, const Process &proc);
    friend std::istream &operator>>(std::istream &is, Process &proc);

private:
    bool verbose;
    pid_t m_pid;
    int m_fd[4];
    FILE* m_pout;
    FILE* m_pin;
};

#endif
