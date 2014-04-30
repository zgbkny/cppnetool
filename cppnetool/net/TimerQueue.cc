#include <cppnetool/net/TimerQueue.h>

#include <sys/timerfd.h>

namespace cppnetool
{
namespace detail
{

int createTimerfd()
{
	int timerfd = ::timerfd_create(CLOCK_MONOTONIC, TFD_NONBLOCK | TFD_CLOEXEC);

	if (timerfd < 0)
	{

	}
	return timerfd;
}

}




}
