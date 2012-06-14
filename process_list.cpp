#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <iostream>
#include <sstream>

//#include "AgentList.hpp"
#include "ProcessList.hpp"
#include "Process.hpp"

int main(int argc, char *argv[])
{
    using namespace std;

    bool verbose=true;
    ProcessStruct::arg_type args;
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
	    Process myproc = Process(args,verbose);
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

    /*
    ProcessList m_list(N, args, verbose);
    ProcessList::const_iterator end = m_list.end();
    
    unsigned int nn=0;
    while (getline(cin,line))
    {
	ProcessList::iterator it = m_list.begin();
	while(it!=end)
	{
	    (*it)->write("hello world\n");
	    string line = (*it)->read();
	    cerr << "Read line: " << line << endl;
	    ++it;
	}
    }
    */
    return(EXIT_SUCCESS);
}
