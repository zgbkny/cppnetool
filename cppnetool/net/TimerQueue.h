#ifndef CPPNETOOL_NET_TIMERQUEUE_H
#define CPPNETOOL_NET_TIMERQUEUE_H

#include <set>
#include <vector>
#include <cppnetool/net/EventLoop.h>
#include <cppnetool/net/Timer.h>
#include <cppnetool/net/TimerId.h>
#include <cppnetool/net/Channel.h>
#include <cppnetool/net/Callbacks.h>

namespace cppnetool
{
class EventLoop;
class Timer;
class TimerId;

class TimerQueue
{
public:
	TimerQueue(EventLoop* loop);
	~TimerQueue();

	///
	/// Schedules the callback to be run at given time,
	/// repeats if @c interval > 0.0.
	///
	/// Must be thread safe. Usually be called from other threads.
	TimerId addTimer(const TimerCallback& cb, Timestamp when, double interval);

  	// void cancel(TimerId timerId);

private:

	// FIXME: use unique_ptr<Timer> instead of raw pointers.
	typedef std::pair<Timestamp, Timer*> Entry;
	typedef std::set<Entry> TimerList;

	void addTimerInLoop(Timer* timer);
	// called when timerfd alarms
	void handleRead();
	// move out all expired timers
	std::vector<Entry> getExpired(Timestamp now);
	void reset(const std::vector<Entry>& expired, Timestamp now);

	bool insert(Timer* timer);

	EventLoop* loop_;
	const int timerfd_;
	net::Channel timerfdChannel_;
	// Timer list sorted by expiration
	TimerList timers_;
};

}



#endif /*CPPNETOOL_NET_TIMERQUEUE_H*/