#include <cstdlib>
#include <cstdio>
#include <unistd.h>
#include <iostream>
#include <sstream>
#include <memory>
#include "Process.hpp"
#include "Pipeline.hpp"

int main(int argc, char *argv[])
{
    using namespace std;
    //Process* myproc;
    ProcessStruct::arg_type args;

    for(int nn=1; nn<argc; ++nn)
    {
	std::cerr << "Arg: " << argv[nn] << std::endl;
	args.push_back(argv[nn]);
    }
    //args.push_back( NULL );

    
    string line;
    string output;

    for(int nn=0; nn < 2; ++ nn)
    {
	unique_ptr<Process> myproc(new Process(args,true));
	//Process myproc(args,true);
	
	for(int n=0; n<5; ++n) {
	
	    stringstream ss(line);
	    ss << "1+" << n << endl;
	    line = ss.str();
	    cout << "calling write with line=" << line << "END" << endl;
	    myproc->write(line);
	    output = myproc->read();
	    cout << "output from process: " << output << "END" << endl;
	}

	//myproc->close_output();
    }
    //delete myproc;
    cerr << "Program exiting" << endl;
    return(EXIT_SUCCESS);
}
