#ifndef M2_HPP
#define M2_HPP

#include <string>
#include <vector>
#include <utility>
#include <zmq.hpp>
#include <climits>
#include <stdint.h>

struct pollfd;

namespace m2pp {

const size_t MAX_IDENTS(100);

typedef std::pair<std::string, std::string> header;

struct request {
	std::string sender;
	std::string conn_id;
	std::string path;
	std::vector<header> headers;
	std::string body;
	bool disconnect;
	static request parse(zmq::message_t& msg);
};

class connection {
	public:
		connection(const std::string& sender_id_, const std::string& sub_addr_, const std::string& pub_addr_);
		~connection();
		int poll(struct pollfd *pollfds, size_t pollfdsNum);
		request recv();
		void reply(const request& req, const std::string& response);
		void reply_http(const request& req, const std::string& response, uint16_t code = 200, const std::string& status = "OK", std::vector<header> hdrs = std::vector<header>());
		void reply_websocket(const request& req, const std::string& response, char opcode=1, char rsvd=0);
		void deliver(const std::string& uuid, const std::vector<std::string>& idents, const std::string& data);
		void deliver_websocket(const std::string& uuid, const std::vector<std::string>& idents, const std::string& data, char opcode=1, char rsvd=0);
	private:
		zmq::context_t ctx;
		std::string sender_id;
		std::string sub_addr;
		std::string pub_addr;
		zmq::socket_t reqs;
		zmq::socket_t resp;
};

}


#endif
