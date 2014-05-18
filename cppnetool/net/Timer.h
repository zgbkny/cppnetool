#ifndef CPPNETOOL_NET_TIMER_H
#define CPPNETOOL_NET_TIMER_H

#include <cppnetool/net/Callbacks.h>
#include <cppnetool/base/Timestamp.h>

namespace cppnetool
{
namespace net
{
class Timer
{
public:
	Timer(const TimerCallback& cb, Timestamp when, double interval)
	    : 	callback_(cb),
	    	expiration_(when),
	    	interval_(interval),
	    	repeat_(interval > 0.0),
	    	sequence_(s_numCreated_.incrementAndGet())
  	{ }

	void run() const
	{
		callback_();
	}

	Timestamp expiration() const  { return expiration_; }
	bool repeat() const { return repeat_; }

	void restart(Timestamp now);

private:
	const TimerCallback callback_;
	Timestamp expiration_;
	const double interval_;
	const bool repeat_;
	const int64_t sequence_;

	static AtomicInt64 s_numCreated_;
};
}
}

#endif /*CPPNETOOL_NET_TIMER_H*/