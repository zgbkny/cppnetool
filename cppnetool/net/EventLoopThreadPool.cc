#include <cppnetool/net/EventLoopThreadPool.h>
#include <cppnetool/base/debug.h>
#include <cppnetool/net/EventLoop.h>
#include <cppnetool/net/EventLoopThread.h>

using namespace cppnetool;
using namespace cppnetool::net;

EventLoopThreadPool::EventLoopThreadPool(EventLoop *baseLoop)
	:	baseLoop_(baseLoop),
		started_(false),
		numThreads_(0),
		next_(0)
{

}

EventLoopThreadPool::~EventLoopThreadPool()
{
	// Don't delete loop, it's stack variable
}

void EventLoopThreadPool::start()
{
	assert(!started_);
	baseLoop_->assertInLoopThread();

	started_ = true;

	for (int i = 0; i < numThreads_; ++i) {
		EventLoopThread *t = new EventLoopThread;
		threads_.push_back(t);
		loops_.push_back(t->startLoop());
	}
}

EventLoop *EventLoopThreadPool::getNextLoop()
{
	baseLoop_->assertInLoopThread();
	EventLoop *loop = baseLoop_;

	if (!loops_.empty()) {
		//round-robin
		loop = loops_[next_];
		++next_;
		if (next_ >= loops_.size()) {
			next_ = 0;
		}
	}
	return loop;
}