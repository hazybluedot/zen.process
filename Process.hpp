#ifndef _ZEN_PROCESS_HPP_
#define _ZEN_PROCESS_HPP_

#include <sys/types.h>
#include <sys/poll.h>
#include <iostream>
#include <vector>

#define	PARENT_READ     m_readpipe[0]
#define	CHILD_WRITE	m_readpipe[1]
#define CHILD_READ      m_writepipe[0]
#define PARENT_WRITE	m_writepipe[1]

class Process
{
public:
    Process(const std::vector<char*>&, bool verbose);
    Process(const std::vector<char*>&);
    Process(const Process &p);
    Process& operator=(const Process &p);
    Process(Process&& other);
    Process& operator=(Process&& other);

    ~Process();

    void write(const std::string&);
    std::string read();
    void close_output();
    int wait();

    friend std::ostream &operator<<(std::ostream& os, const Process &proc);
    friend std::istream &operator>>(std::istream &is, Process &proc);

private:
    bool verbose;
    std::string m_name;
    pid_t m_pid;
    int m_writepipe[2];
    int m_readpipe[2];
    FILE* m_pout;
    FILE* m_pin;
    struct pollfd m_fds[1];
    char *m_instring;
    int m_status;

    void create(const std::vector<char*>&);
    int close_stream(FILE*);
};

#endif
