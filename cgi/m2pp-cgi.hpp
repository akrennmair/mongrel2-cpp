#ifndef M2PP_CGI_HPP
#define M2PP_CGI_HPP

#include <m2pp.hpp>
#include <string>

void handle_request(m2pp::connection& conn, m2pp::request& req, const std::string& cgidir);

enum loglevel { ERROR = 0, WARN, INFO, DEBUG };

void logmsg(loglevel l, const char * s, ...);

#endif
