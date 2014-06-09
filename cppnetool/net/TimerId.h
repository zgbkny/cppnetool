#ifndef CPPNETOOL_NET_TIMERID_H
#define CPPNETOOL_NET_TIMERID_H

#include <cppnetool/net/Timer.h>

namespace cppnetool
{
namespace net
{
class Timer;

class TimerId
{
public:
	explicit TimerId(Timer *timer = NULL, int64_t seq = 0)
		:	timer_(timer),
			sequence_(seq)
	{

	}

	friend class TimerQueue;

private:
	Timer *timer_;
	int64_t sequence_;
};
}
}

#endif /*CPPNETOOL_NET_TIMERID_H*/