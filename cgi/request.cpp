#include "m2pp-cgi.hpp"

void handle_request(m2pp::connection& conn, m2pp::request& req) {
	conn.reply_http(req, "hello, world!");
	// TODO: implement
}
