#ifndef CPPNETOOL_NET_TIMERID_H
#define CPPNETOOL_NET_TIMERID_H

#include <cppnetool/net/Timer.h>

namespace cppnetool
{
class Timer;

class TimerId
{
public:
	explicit TimerId(Timer *timer)
		: value_(timer)
	{

	}

private:
	Timer *value_;
};

}

#endif /*CPPNETOOL_NET_TIMERID_H*/