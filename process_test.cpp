#include <cstdlib>
#include <cstdio>
#include <unistd.h>
#include <iostream>
#include <sstream>
#include <memory>
#include "Process.hpp"

int main(int argc, char *argv[])
{
    using namespace std;
    //Process* myproc;
    std::vector<char*> args;

    for(int nn=1; nn<argc; ++nn)
    {
	std::cerr << "Arg: " << argv[nn] << std::endl;
	args.push_back(argv[nn]);
    }
    args.push_back( NULL );

    
    string line;
    string output;
    //try {
	auto_ptr<Process> myproc(new Process(args));
	//} catch (std::string s)
	/* {
	cerr << "Exception thrown: " << s << endl;
	return(EXIT_FAILURE);
	}*/

    for(int n=0; n<5; ++n) {

	stringstream ss(line);
	ss << "This is test " << n << endl;
	line = ss.str();
	cerr << "calling write with line=" << line << "END" << endl;
	myproc->write(line);
    }

    myproc->close_output();
    output = myproc->read();
    cerr << "output from process: " << output << "END" << endl;
    
    //delete myproc;
    cerr << "Program exiting" << endl;
    return(EXIT_SUCCESS);
}
