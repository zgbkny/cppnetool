#ifndef CPPNETOOL_NET_EVENTLOOPTHREAD_H
#define CPPNETOOL_NET_EVENTLOOPTHREAD_H
#include <cppnetool/base/Thread.h>
#include <cppnetool/base/Mutex.h>
#include <cppnetool/base/Condition.h>
#include <cppnetool/net/EventLoop.h>


namespace cppnetool
{
namespace net
{
class EventLoopThread
{
public:
	EventLoopThread();
	~EventLoopThread();
	EventLoop* startLoop();

private:
	void threadFunc();

	EventLoop* loop_;
	bool exiting_;
	Thread thread_;
	MutexLock mutex_;
	Condition cond_;
};
}
}



#endif /*CPPNETOOL_NET_EVENTLOOPTHREAD_H*/