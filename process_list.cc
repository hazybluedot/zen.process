#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <iostream>
#include <sstream>

#include "AgentList.hpp"
#include "Process.hpp"

int main(int argc, char *argv[])
{
    using namespace std;

    std::vector<char*> args;
    int N = atoi(argv[1]);
    for(int nn=2; nn<argc; ++nn)
    {
	std::cerr << "Arg: " << argv[nn] << std::endl;
	args.push_back(argv[nn]);
    }
    args.push_back( NULL );
    if (N < 1)
    {
	cerr << "N must be 1 or greater" << endl;
	exit(1);
    }

    string line;
    if (N == 1)
    {
	cerr << "Creating Process" << endl;
	try {
	    Process myproc = Process(args);
	    while (getline(cin,line))
	    {
		//stringstream ss(line, stringstream::out);
		//ss >> myproc;
		myproc.write(line);
		string line = myproc.read();
		cout << "Got a line: " << line << endl;
	    }
	} catch (std::string s) {
	    cerr << "Error creating process: " << s << endl;
	    exit(1);
	}
	return(EXIT_SUCCESS);
    }

    AgentList m_list(N, args);
    AgentList::const_iterator end = m_list.end();
    unsigned int nn=0;
    while (getline(cin,line))
    {
	AgentList::iterator it = m_list.begin();
	while(it!=end)
	{
	    int fd = it->second;
	    FILE* stream  = fdopen(fd, "w");
	    //std::ostream fstream(stream);
	    std::cerr << "Attempting to write to fd " << fd << std::endl;
	    fprintf(stream, "test %d\n", nn++);
	    
	    //stream << "test " << nn << std::endl;
	    //write(fd, "test\n", 6);
	    fflush(stream);
	    //close(fd);
	    ++it;
	}
    }
    return(EXIT_SUCCESS);
}
