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

    for(int nn=0; nn < 2; ++nn)
    {
	{
	    std::unique_ptr<Pipeline> mypipe_ptr(new Pipeline(argsv,verbose));
	    //Pipeline& mypipe(*mypipe_ptr);
	
	for(int n=0; n<10; ++n) {
	    stringstream ss(line);
	    ss << "1+" << n << endl;
	    line = ss.str();
	    cerr << "calling write with line=" << line << "END" << endl;
	    mypipe_ptr->write(line);
	    output = mypipe_ptr->read();
	    cerr << "output from process: " << output << "END" << endl;
	}
	//sleep(5);
	cerr << "Pipeline going out of scope" << endl;
	}
    }
    sleep(5);
    cerr << "Program exiting" << endl;
    return(EXIT_SUCCESS);
}
