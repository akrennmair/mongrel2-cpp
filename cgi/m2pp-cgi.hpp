#ifndef M2PP_CGI_HPP
#define M2PP_CGI_HPP

#include <m2pp.hpp>
#include <string>

void handle_request(m2pp::connection& conn, m2pp::request& req, const std::string& cgidir);

#endif
