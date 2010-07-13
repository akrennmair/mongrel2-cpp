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


#if 0

int main(void) {
	std::string identity("82209006-86FF-4982-B5EA-D1E29E55D481");
	zmq::context_t ctx(2);

	zmq::socket_t reqs(ctx, ZMQ_UPSTREAM);
	reqs.connect("tcp://127.0.0.1:9999");

	zmq::socket_t resp(ctx, ZMQ_PUB);
	resp.connect("tcp://127.0.0.1:9998");
	resp.setsockopt(ZMQ_IDENTITY, identity.data(), identity.length());

	while (1) {
		zmq::message_t inmsg;
		reqs.recv(&inmsg);

		std::string result(static_cast<const char *>(inmsg.data()), inmsg.size());

		std::cout << "got " << result << std::endl;

		// TODO: parse "sender conn_id path reset"

		std::string reply("HTTP/1.1 200 OK\r\nContent-Type: text/plain\r\nContent-Length: 8\r\n\r\nOH HAI!\n");

		zmq::message_t outmsg(reply.length());
		::memcpy(outmsg.data(), reply.data(), reply.length());
		resp.send(outmsg);
	}

	return 0;
}
#endif
