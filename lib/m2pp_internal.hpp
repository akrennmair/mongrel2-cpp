#ifndef M2_INT_HPP
#define M2_INT_HPP

namespace m2pp {

	namespace utils {

		std::vector<std::string> split(const std::string& str, const std::string& sep, unsigned int count = 0);
		std::string parse_netstring(const std::string& str, std::string& rest);
		std::vector<header> parse_json(const std::string& jsondoc);

	}

}

#endif
