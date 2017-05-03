/*
(c) 2016,2017 Grain

This file is part of ICSSdroid.

ICSSdroid is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

ICSSdroid is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with ICSSdroid.  If not, see <http://www.gnu.org/licenses/>.
*/

#pragma once


#include <iostream>

namespace ICSE{
namespace log{
	extern std::string removeCallRule(const std::string &str);

	/*extern void log_out(void);
	

	template<typename First, typename... _Args>
	static void log_out(const First &first, const _Args&... args)
	{
		std::cout << first;
		log_out(args...);
	}*/

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-function"
	static void except_msg(std::stringstream &msg) noexcept
	{
	}

	template<typename First, typename..._Args>
	static void except_msg(std::stringstream &msg, const First& first, const _Args&... args) noexcept
	{
		msg << first;
		except_msg(msg, args...);
	}

	template<typename _Throwable, typename... _Args>
	static void except_throw(const std::string &exname, const std::string &file, int line, const std::string &funcname, const _Args&... args) noexcept(false)
	{
		std::stringstream res, msg;
		except_msg(msg, args...);
		res << exname << ": " << msg.str() << std::endl << "      at " << removeCallRule(funcname) << "(" << /*boost::filesystem::path(file).filename().string()*/ file << ":" << line << ")" << std::endl;
		throw _Throwable(res.str().c_str());
	}
#pragma GCC diagnostic pop
}
}

#ifdef DO_LOG
#define LOG(...) (ICSE::log::log_out(__VA_ARGS__))
#else
#define LOG(...) (1)
#endif

#ifdef _MSC_VER
#define THROW(ex, ...) (ICSE::log::except_throw<ex>(#ex, __FILE__, __LINE__, __FUNCSIG__, __VA_ARGS__))
#else
#define THROW(ex, ...) (ICSE::log::except_throw<ex>(#ex, __FILE__, __LINE__, __func__, __VA_ARGS__))
#endif