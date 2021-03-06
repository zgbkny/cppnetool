#ifndef CPPNETOOL_NET_EVENTLOOPTHREADPOOL_H
#define CPPNETOOL_NET_EVENTLOOPTHREADPOOL_H

#include <vector>

namespace cppnetool
{
namespace net
{

class EventLoop;
class EventLoopThread;

class EventLoopThreadPool
{
public:
	EventLoopThreadPool(EventLoop *baseLoop);
	~EventLoopThreadPool();
	void setThreadNum(int numThreads) { numThreads_ = numThreads; }
	void start();
	EventLoop *getNextLoop();

private:
	EventLoop *baseLoop_;
	bool started_;
	int numThreads_;
	unsigned int next_;
	std::vector<EventLoopThread *> threads_;
	std::vector<EventLoop *> loops_;
};

}
}

#endif /*CPPNETOOL_NET_EVENTLOOPTHREADPOOL_H*/