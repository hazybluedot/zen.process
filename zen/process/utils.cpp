#include <iostream>

#include <string.h>

#include <algorithm>
#include <iterator>
#include "utils.hpp"
#include <boost/filesystem.hpp>
namespace fs = boost::filesystem;

namespace zen {
  namespace process {
    std::string basename(const vecstr& args)
    {
      fs::path pathname(args[0]);
      std::string basename = pathname.filename().native();
      return basename;
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

    std::ostream& operator<<(std::ostream& os, const argv_type& argsv)
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

    std::vector<std::string> get_log_string(const std::string &procname, const std::string &name, opts_type &options)
    {
      std::vector<std::string>  logger;
      opts_type::const_iterator oit = options.find(name);
      if (oit != options.end())
	{
	  std::string logname = oit->second;
	  if (logname == "")
	    {
	      logname = procname;
	    }
	  logger = {"tee", "-a", logname + "."+ oit->first};
	  options.erase(oit);
	}
      return logger;
    };

    argov_type process_options(const argv_type &args_, const opts_type &options_)
    {
      argov_type argsv;

      std::for_each(args_.begin(), args_.end(), [&](const std::vector<std::string> &args) 
		    {
		      opts_type options = options_;
		      std::string ids = "";
		      std::vector<std::string> inlogger;
		      std::vector<std::string> outlogger;

		      std::map<std::string,std::string>::const_iterator oit;
		      oit = options.find("id");
		      if (oit != options.end())
			{
			  ids = oit->second;
			}

		      inlogger = get_log_string(args[0], "stdin", options);
		      outlogger = get_log_string(args[0], "stdout", options);
		  
		      if (inlogger.size() > 0)
			argsv.push_back(std::make_pair(inlogger, opts_type()));

		      argsv.push_back(std::make_pair(args, options));

		      if (outlogger.size() > 0)
			argsv.push_back(std::make_pair(outlogger, opts_type()));
		  
		    } );
      return argsv;
    }
  }
}
