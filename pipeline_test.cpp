#include <cstdlib>
#include <cstdio>
#include <unistd.h>
#include <iostream>
#include <sstream>
#include <memory>
#include <stdexcept>
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

    std::vector<std::string> inargs;
    inargs.push_back( "tee");
    inargs.push_back( "pipeline_test.in.log");

    argsv.push_back(inargs);

    argsv.push_back(args);
    
    args.clear();
    args.push_back( "cat");
    //args.push_back( "pipeline_test.out.log");

    argsv.push_back(args);

    string line;
    string output;

    for(int nn=0; nn < 1; ++nn)
    {
	
	try {
	    std::unique_ptr<Pipeline> mypipe_ptr(new Pipeline(argsv,verbose));
	    //Pipeline& mypipe(*mypipe_ptr);
	    sleep(1);
	    for(int n=0; n<3; ++n) {
		stringstream ss(line);
		ss << "1+" << n << endl;
		line = ss.str();
		cerr << "calling write with line=" << line << "END" << endl;
		mypipe_ptr->write(line);
		output = mypipe_ptr->read();
		cerr << "output from process: " << output << "END" << endl;
	    }
	    cerr << "Pipeline going out of scope" << endl;
	} catch (std::runtime_error e) 
	{
	    std::cerr << "Got error: " << e.what() << std::endl;
	    return(1);
	}
    }
    sleep(5);
    cerr << "Program exiting" << endl;
    return(EXIT_SUCCESS);
}
