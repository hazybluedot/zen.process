#include <cstdlib>
#include <cstdio>
#include <unistd.h>
#include <iostream>
#include <sstream>
#include <memory>
#include "Pipeline.hpp"

int main(int argc, char *argv[])
{
    using namespace std;
    
    bool verbose=true;
    Process::arg_type args;

    for(int nn=1; nn<argc; ++nn)
    {
	std::cerr << "Arg: " << argv[nn] << std::endl;
	args.push_back(argv[nn]);
    }
    args.push_back( NULL );

    Pipeline::arg_type argsv;
    argsv.push_back(args);
    
    args.clear();
    args.push_back( "tee");
    args.push_back( "pipeline_test.log");

    argsv.push_back(args);

    args.clear();
    args.push_back( "tee");
    args.push_back( "pipeline_test2.log");

    argsv.push_back(args);

    string line;
    string output;

    {
	Pipeline mypipe(argsv,verbose);
	
	for(int n=0; n<5; ++n) {
	    stringstream ss(line);
	    ss << "1+" << n << endl;
	    line = ss.str();
	    cerr << "calling write with line=" << line << "END" << endl;
	    mypipe.write(line);
	    output = mypipe.read();
	    cerr << "output from process: " << output << "END" << endl;
	}
	sleep(5);
	cerr << "Pipeline going out of scope" << endl;
    }
    sleep(5);
    cerr << "Program exiting" << endl;
    return(EXIT_SUCCESS);
}
