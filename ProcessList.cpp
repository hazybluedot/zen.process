#include "ProcessList.hpp"

ProcessList::ProcessList(int N,  const std::vector<char*>& args, bool verbose)
{
    for(int nn=0; nn<N; ++nn)
    {
	//std::unique_ptr<Process> ptr(new Process(args));
	m_ProcessList.emplace_back(std::unique_ptr<Process>(new Process(args)));
    }
}

ProcessList::~ProcessList() {};
