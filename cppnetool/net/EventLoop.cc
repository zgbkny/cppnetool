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
	quit_ = false;
	// poll
	while (!quit_)
	{
		activeChannels_.clear();
		poller_->poll(kPollTimeMs, &activeChannels_);
		for (ChannelList::iterator it = activeChannels_.begin();
			it != activeChannels_.end(); ++it)
		{
			(*it)->handleEvent();
		}
	}

	looping_ = false;
}

void EventLoop::quit()
{
	quit_ = true;
}

void EventLoop::abortNotInLoopThread()
{
	std::cout << "EventLoop::abortNotInLoopThread - EventLoop " << this
            << " was created in threadId_ = " << threadId_
            << ", current thread id = " <<  CurrentThread::tid();
}

void EventLoop::updateChannel(Channel *channel)
{
	assert(channel->ownerLoop() == this);
	assertInLoopThread();
	poller_->updateChannel(channel);
}


