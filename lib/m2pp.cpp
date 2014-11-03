#include <string>
#include <sstream>
#include <zmq.hpp>
#include <assert.h>
#include "m2pp.hpp"
#include "m2pp_internal.hpp"

namespace m2pp {

connection::connection(const std::string& sender_id_, const std::string& sub_addr_, const std::string& pub_addr_) 
	: ctx(1), sender_id(sender_id_), sub_addr(sub_addr_), pub_addr(pub_addr_), reqs(ctx, ZMQ_PULL), resp(ctx, ZMQ_PUB) {
	reqs.connect(sub_addr.c_str());
	resp.connect(pub_addr.c_str());
	resp.setsockopt(ZMQ_IDENTITY, sender_id.data(), sender_id.length());
}

connection::~connection() {
}

request connection::recv() {
	zmq::message_t inmsg;
	reqs.recv(&inmsg);
	return request::parse(inmsg);
}

void connection::reply_http(const request& req, const std::string& response, uint16_t code, const std::string& status, std::vector<header> hdrs) {
	std::ostringstream httpresp;

	httpresp << "HTTP/1.1" << " " << code << " " << status << "\r\n";
	httpresp << "Content-Length: " << response.length() << "\r\n";
	for (std::vector<header>::iterator it=hdrs.begin();it!=hdrs.end();++it) {
		httpresp << it->first << ": " << it->second << "\r\n";
	}
	httpresp << "\r\n" << response;

	reply(req, httpresp.str());
}

void connection::reply(const request& req, const std::string& response) {
	// Using the new mongrel2 format as of v1.3
	std::ostringstream msg;
	msg << req.sender << " " << req.conn_id.size() << ":" << req.conn_id << ", " << response;
	std::string msg_str = msg.str();
	zmq::message_t outmsg(msg_str.length());
	::memcpy(outmsg.data(), msg_str.data(), msg_str.length());
	resp.send(outmsg);
}

void connection::reply_websocket(const request& req, const std::string& response, char opcode, char rsvd) {
    reply(req, utils::websocket_header(response.size(), opcode, rsvd) + response);
}

void connection::deliver(const std::string& uuid, const std::vector<std::string>& idents, const std::string& data) {
	assert(idents.size() <= MAX_IDENTS);
	std::ostringstream msg;
	msg << uuid << " ";

	size_t idents_size(idents.size()-1); // initialize with size needed for spaces
	for (size_t i=0; i<idents.size(); i++) {
		idents_size += idents[i].size();
	}
	msg << idents_size << ":";
	for (size_t i=0; i<idents.size(); i++) {
		msg << idents[i];
		if (i < idents.size()-1)
			msg << " ";
	}
	msg << ", " << data;

	std::string msg_str = msg.str();
	zmq::message_t outmsg(msg_str.length());
	::memcpy(outmsg.data(), msg_str.data(), msg_str.length());
	resp.send(outmsg);
}

void connection::deliver_websocket(const std::string& uuid, const std::vector<std::string>& idents, const std::string& data, char opcode, char rsvd) {
	deliver(uuid, idents, utils::websocket_header(data.size(), opcode, rsvd) + data);
}

request request::parse(zmq::message_t& msg) {
	request req;
	std::string result(static_cast<const char *>(msg.data()), msg.size());

	std::vector<std::string> results = utils::split(result, " ", 3);

	req.sender = results[0];
	req.conn_id = results[1];
	req.path = results[2];

	std::string body;
	std::string ign;

	req.headers = utils::parse_json(utils::parse_netstring(results[3], body));

	req.body = utils::parse_netstring(body, ign);

	//check disconnect flag
	req.disconnect = false;
	for (std::vector<header>::const_iterator it = req.headers.begin();
			it != req.headers.end(); ++it) {
		if (it->first == "METHOD" && it->second == "JSON" &&
				req.body == "{\"type\":\"disconnect\"}") {
			req.disconnect = true;
			break;
		}
	}

	return req;
}

}
