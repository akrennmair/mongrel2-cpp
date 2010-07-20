#include "m2pp-cgi.hpp"

#include <cstdio>
#include <cstdarg>
#include <cstdlib>
#include <iostream>

const char * levelname[] = { "ERROR", "WARN", "INFO", "DEBUG", NULL };

void logmsg(loglevel l, const char * s, ...) {
	char * msg = NULL;
	va_list ap;

	va_start(ap, s);
	vasprintf(&msg, s, ap);
	va_end(ap);
	std::cerr << levelname[l] << ": " << msg << std::endl;
	free(msg);
}
