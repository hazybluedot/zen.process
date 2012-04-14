#include <iostream>

#include <string.h>

#include <algorithm>
#include <iterator>
#include "utils.hpp"

vecvecstr make_log_args(const vecstr& args, std::string logname)
{

    vecvecstr argsv;
    argsv.push_back(args);

    vecstr logargs;
    logargs.push_back( "tee" );
    logargs.push_back( logname );
    argsv.emplace_back(logargs);
    return argsv;
}

const char *convert(const std::string & s)
{
   return s.c_str();
}

std::vector<const char*> convert_vs2vc(const std::vector<std::string>& vs)
{
    std::vector<const char*> vc;
    std::transform(vs.begin(), vs.end(), std::back_inserter(vc), convert);
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
