#include <iostream>

#include <string.h>

#include <algorithm>
#include <iterator>
#include "utils.hpp"
#include <boost/filesystem.hpp>
namespace fs = boost::filesystem;

std::string basename(const vecstr& args)
{
    fs::path pathname(args[0]);
    std::string basename = pathname.filename().native();
    return basename;
}


vecvecstr add_output_logger(const vecvecstr& pipeline, const std::string& lname)
{
    std::string logname = lname;
    if (logname == "")
	logname = pipeline.back()[0].c_str();
    logname.append(".stdout");
    std::cerr << "Adding output logger " << logname << std::endl;

    vecvecstr argsv = pipeline;
    vecstr logargs = {"tee", "-a", logname};
    argsv.emplace_back(logargs);
    return argsv;
}

vecvecstr add_input_logger(const vecvecstr& pipeline, const std::string& lname)
{
    std::string logname = lname;
    if (logname == "")
	logname = pipeline.front()[0].c_str();
    logname.append(".stdin");

    std::cerr << "Adding input logger " << logname << std::endl;
    vecvecstr argsv;
    vecstr logargsin = {"tee", "-a", logname};
    argsv.emplace_back(logargsin);
    std::copy(pipeline.begin(),pipeline.end(), std::back_inserter<vecvecstr>(argsv));
//argsv.emplace_back(pipeline);
    return argsv;
}

std::vector<const char*> convert_vs2vc(const std::vector<std::string>& vs)
{
    std::vector<const char*> vc;
    std::transform(vs.begin(), vs.end(), std::back_inserter(vc), [](const std::string s)
		   {
		       return s.c_str();
		   } );
    vc.push_back( NULL );
    return vc;
};

std::ostream& operator<<(std::ostream& os, const std::vector<const char*>& args)
{
    
    //for_each(args.begin(), args.end(), [&os]
    std::copy(args.begin(), args.end(), std::ostream_iterator<const char*>(os, "\n"));
    //os << const_cast<char**>( &args[0] );
    return os;
};

std::ostream& operator<<(std::ostream& os, const vecstr& args)
{
    
    //for_each(args.begin(), args.end(), [&os]
    std::copy(args.begin(), args.end(), std::ostream_iterator<std::string>(os, "\n"));
    //os << const_cast<char**>( &args[0] );
    return os;
};

std::ostream& operator<<(std::ostream& os, const vecvecstr& argsv)
{
    unsigned int nn=0;
    for_each(argsv.begin(), argsv.end(), [&nn,&os](vecstr args)
	     {
		 os << "argsv[" << nn++ << "]:" << std::endl;
		 os << args << std::endl;
	     }
	);

    return os;
}
