#ifndef CPPNETOOL_BASE_LOGGING_H
#define CPPNETOOL_BASE_LOGGING_H

#include <cppnetool/base/debug.h>

namespace cppnetool
{

class Logger
{
public:
	enum {
		TRACE,
		DEBUG,
		INFO,
		WARN,
		ERROR,
		FATAL,
		NUM_LOG_LEVELS,
	};
	
};

#define LOG_TRACE std::cout
#define LOG_DEBUG std::cout
#define LOG_INFO std::cout
#define LOG_WARN std::cout
#define LOG_ERROR std::cout
#define LOG_FATAL std::cout
#define LOG_SYSERR std::cout
#define LOG_SYSFATAL std::cout

}

#endif /*CPPNETOOL_BASE_LOGGING_H*/