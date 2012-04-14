
#include "ProcessList.hpp"

ProcessList::ProcessList(int N,  const Process::arg_type& args, bool verbose)
{
    for(int nn=0; nn<N; ++nn)
    {
	//std::unique_ptr<Process> ptr(new Process(args));
	m_ProcessList.emplace_back(std::unique_ptr<Process>(new Process(args,verbose)));
    }
}

ProcessList::~ProcessList() {};

void ProcessList::sigchld_handler(int signo)
{
    int old_errno = errno;
    while (1) {
	register int pid;
	int w;
	
	/* keep asking for status until we get a result */
	do
	{
	    errno = 0;
	    pid = waitpid(WAIT_ANY, &w, WNOHANG | WUNTRACED);
	}
	while (pid <=0 && errno == EINTR);

	if (pid <= 0) {
	    // an error occured
	    errno = old_errno;
	    return;
	}

	/* find the process that signaled us */
	for_each(m_processes.begin(), m_processes.end(), [](std::unique_ptr<Process>& p)
		 {
		     if (p->m_pid == pid) {
			 p->have_status = 1;
			 /* If process has terminated, stop waiting for its output.  */
			 if (WIFSIGNALED (w) || WIFEXITED (w))
			     if (p->input_descriptor)
				 FD_CLR (p->input_descriptor, &input_wait_mask);
			 /* The program should check this flag from time to time
			    to see if there is any news in process_list.  */
			 ++process_status_change;
		     }
		 }
	    );
}
