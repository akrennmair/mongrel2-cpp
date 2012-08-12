#include <string>
#include <vector>
#include <sstream>
#include <json/json.h>
#include "m2pp.hpp"
#include "m2pp_internal.hpp"

namespace m2pp {

namespace utils {

std::vector<std::string> split(const std::string& str, const std::string& sep, unsigned int count) {
	std::vector<std::string> result;
	std::string::size_type last_pos = str.find_first_not_of(sep, 0);
	std::string::size_type pos = str.find_first_of(sep, last_pos);
	int i = count;

	while (std::string::npos != pos || std::string::npos != last_pos) {
		result.push_back(str.substr(last_pos, pos - last_pos));
		last_pos = str.find_first_not_of(sep, pos);
		pos = str.find_first_of(sep, last_pos);
		if (count > 0) {
			i--;
			if (i==0) {
				result.push_back(str.substr(last_pos, str.length() - last_pos));
				break;
			}
		}
	}

	return result;
}

std::string parse_netstring(const std::string& str, std::string& rest) {
	std::vector<std::string> result = split(str, ":", 1);
	std::istringstream is(result[0]);
	unsigned int len;
	is >> len;
	rest = result[1].substr(len+1, result[1].length() - len);
	return result[1].substr(0, len);
}

std::vector<header> parse_json(const std::string& jsondoc) {
	std::vector<header> hdrs;

	json_object * jobj = json_tokener_parse(jsondoc.c_str());

	if (jobj && json_object_is_type(jobj, json_type_object)) {
		json_object_object_foreach(jobj, key, value) {
			if (key && value && json_object_is_type(value, json_type_string)) {
				hdrs.push_back(header(key, json_object_get_string(value)));
			}
		}
	}

	json_object_put(jobj); // free json object

	return hdrs;
}

std::string websocket_header(size_t data_size, char opcode, char rsvd) {
    std::ostringstream header;
    header.put(0x80|opcode|rsvd<<4);
    char dummyLength;
    size_t realLength=data_size;
    if (realLength < 126) {
        dummyLength = static_cast<char>(realLength);
    } else if (realLength < 1<<16) {
        dummyLength = 126;
    } else {
        dummyLength = 127;
    }
    header.put(dummyLength);
    if (dummyLength == 127) {
        header.put(realLength >> 56 &0xff);
        header.put(realLength >> 48 &0xff);
        header.put(realLength >> 40 &0xff);
        header.put(realLength >> 32 &0xff);
        header.put(realLength >> 24 & 0xff);
        header.put(realLength >> 16 & 0xff);
    } if (dummyLength == 126 || dummyLength == 127) {
        header.put(realLength >> 8 & 0xff);
        header.put(realLength & 0xff);
    }
    return header.str();
}

}

}
