#include <zmq.hpp>
#include <cstring>
#include <string>
#include <iostream>
#include <sstream>
#include "m2pp.hpp"

int main(void) {
	std::string sender_id = "82209006-86FF-4982-B5EA-D1E29E55D481";

	m2pp::connection conn(sender_id, "tcp://127.0.0.1:9999", "tcp://127.0.0.1:9998");

	while (1) {
		m2pp::request req = conn.recv();

		std::ostringstream response;
		response << "<pre>" << std::endl;
		response << "SENDER: " << req.sender << std::endl;
		response << "IDENT: " << req.conn_id << std::endl;
		response << "PATH: " << req.path << std::endl;
		response << "BODY: " << req.body << std::endl;
		for (std::vector<m2pp::header>::iterator it=req.headers.begin();it!=req.headers.end();it++) {
			response << "HEADER: " << it->first << ": " << it->second << std::endl;
		}
		response << "</pre>" << std::endl;

		std::cout << response.str();

		conn.reply_http(req, response.str());
	}

	return 0;
}

