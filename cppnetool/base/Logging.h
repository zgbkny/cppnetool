#ifndef CPPNETOOL_BASE_LOGGING_H
#define CPPNETOOL_BASE_LOGGING_H

namespace
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

}

#endif /*CPPNETOOL_BASE_LOGGING_H*/