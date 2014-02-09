#include <zmq.hpp>
#include <string>
#include <iostream>
#include <sstream>
#include <cstring>
#include <cstdlib>
#include <signal.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <m2pp.hpp>
#include "m2pp-cgi.hpp"

static void sigchld_handler(int sig) {
	if (sig != SIGCHLD)
		return;

	pid_t pid;
	int stat;

	while ((pid = waitpid(-1,&stat,WNOHANG)) > 0) { }
	::signal(SIGCHLD, sigchld_handler); // in case of unreliable signals (read: System V)
}

static void usage(const char * argv0) {
	std::cout << argv0 << ": usage: " << argv0 << " -i <sender_id> -p <pub_addr> -s <sub_addr> -d <basedir> [-c <num_preforked_processes>]" << std::endl;
	::exit(1);
}

int main(int argc, char * argv[]) {
	std::string sender_id;
	std::string pub_addr;
	std::string sub_addr;
	std::string cgidir;
	unsigned long processes = 1;

	int c;

	do {
		if ((c = ::getopt(argc, argv, "i:p:s:d:c:h")) < 0)
			continue;

		switch (c) {
			case ':':
			case '?':
			case 'h':
				usage(argv[0]);
				break;
			case 'i':
				sender_id = optarg;
				break;
			case 'p':
				pub_addr = optarg;
				break;
			case 's':
				sub_addr = optarg;
				break;
			case 'd':
				cgidir = optarg;
				break;
			case 'c': {
					char * endptr;
					processes = strtoul(optarg, &endptr, 10);
					if (endptr == optarg) {
						std::cerr << "Error: invalid parameter `" << optarg << "' for option -c" << std::endl;
						usage(argv[0]);
					}
				}
				break;
			default:
				std::cout << argv[0] << ": unknown option -" << static_cast<char>(c) << std::endl;
				usage(argv[0]);
				break;
		}

	} while (c != -1);

	if (pub_addr == "" || sub_addr == "" || sender_id == "" || cgidir == "") {
		usage(argv[0]);
	}

	::signal(SIGCHLD, sigchld_handler);

	for (unsigned int i=1;i<processes;i++) {
		int rc = fork();
		switch (rc) {
			case 0:
				goto open_conn;
			case -1:
				std::cerr << "fork failed: " << ::strerror(errno) << std::endl;
				break;
			default:
				logmsg(DEBUG, "started process %u (pid %d)", i, rc);
				break;
		}
	}

open_conn:
	m2pp::connection conn(sender_id, pub_addr, sub_addr);

	while (1) {
		m2pp::request req = conn.recv();
		handle_request(conn, req, cgidir);
	}

	return 0;
}
