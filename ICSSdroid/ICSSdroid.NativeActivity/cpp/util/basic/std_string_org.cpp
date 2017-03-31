#include "util/basic/std_string_org.h"

namespace std {
	std::string to_string(int value) {
		std::stringstream ss;
		ss << value;
		return ss.str();
	}
}