#include <iostream>
#include <cppnetool/net/EventLoop.h>


using namespace cppnetool;
using namespace cppnetool::net;

__thread EventLoop* t_loopInThisThread = 0;

EventLoop::EventLoop() 
	: looping_(false), 
	  threadId_(CurrentThread::tid())
{
	if (t_loopInThisThread) {

	} else {
		t_loopInThisThread = this;
	}
}
EventLoop::~EventLoop()
{
	assert(!looping_);
	t_loopInThisThread = NULL;
}
void EventLoop::loop()
{
	assert(!looping_);
	assertInLoopThread();
	looping_ = true;

	// poll

	looping_ = false;
}

void EventLoop::abortNotInLoopThread()
{
	std::cout << "EventLoop::abortNotInLoopThread - EventLoop " << this
            << " was created in threadId_ = " << threadId_
            << ", current thread id = " <<  CurrentThread::tid();
}

void EventLoop::updateChannel(Channel *channel)
{
	
}


