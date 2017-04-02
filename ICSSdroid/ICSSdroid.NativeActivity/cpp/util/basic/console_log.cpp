#include "util/basic/console_log.h"


std::string ICSE::log::removeCallRule(const std::string &str) {
	auto ubpos = str.find("__");
	auto sppos = str.find(' ', ubpos);
	auto bpos = str.find('(');
	if (ubpos != std::string::npos && sppos != std::string::npos && bpos != std::string::npos)
		return str.substr(sppos + 1, bpos - sppos - 1);
	else
		return str;
}

/*void ICSE::log::log_out(void)
{
	std::cout << std::endl;
}*/
