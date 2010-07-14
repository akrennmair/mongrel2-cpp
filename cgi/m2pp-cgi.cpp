#include <zmq.hpp>
#include <string>
#include <iostream>
#include <sstream>
#include <cstring>
#include <cstdlib>
#include <signal.h>
#include <sys/types.h>
#include <sys/wait.h>
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
	std::cout << argv0 << ": usage: " << argv0 << " -i <sender_id> -p <pub_addr> -s <sub_addr>" << std::endl;
	::exit(1);
}

int main(int argc, char * argv[]) {
	std::string sender_id;
	std::string pub_addr;
	std::string sub_addr;
	int c;

	do {
		if ((c = ::getopt(argc, argv, "i:p:s:h")) < 0)
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
			default:
				std::cout << argv[0] << ": unknown option -" << static_cast<char>(c) << std::endl;
				usage(argv[0]);
				break;
		}

	} while (c != -1);

	if (pub_addr == "" || sub_addr == "" || sender_id == "") {
		usage(argv[0]);
	}

	::signal(SIGCHLD, sigchld_handler);

	m2pp::connection conn(sender_id, pub_addr, sub_addr);

	while (1) {
		m2pp::request req = conn.recv();

		int rc = fork();
		switch (rc) {
			case 0:
				handle_request(conn, req);
				::exit(0);
				break;
			case -1:
				std::cerr << "fork failed: " << ::strerror(errno) << std::endl;
				break;
			default:
				break;
		}
	}

	return 0;
}
