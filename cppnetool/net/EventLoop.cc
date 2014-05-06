#include <cppnetool/base/debug.h>
#include <cppnetool/net/EventLoop.h>
#include <cppnetool/net/Poller.h>
#include <cppnetool/net/Channel.h>
#include <cppnetool/base/Logging.h>
#include <sys/eventfd.h>
#include <unistd.h>



using namespace cppnetool;
using namespace cppnetool::net;

__thread EventLoop* t_loopInThisThread = 0;
const int kPollTimeMs = 10000;

static int createEventfd()
{
	int evtfd = ::eventfd(0, EFD_NONBLOCK | EFD_CLOEXEC);
	if (evtfd < 0)
	{
		LOG_SYSERR << "Failed in eventfd";
		abort();
	}
	return evtfd;
}

EventLoop::EventLoop() 
	: looping_(false), 
	  threadId_(CurrentThread::tid()),
	  poller_(new Poller(this)),
	  wakeupFd_(createEventfd())
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
            << ", current thread id = " <<  CurrentThread::tid() << std::endl;
}

void EventLoop::wakeup()
{
	uint64_t one = 1;
	ssize_t n = ::write(wakeupFd_, &one, sizeof one);
	if (n != sizeof one)
	{
		LOG_ERROR << "EventLoop::wakeup() writes " << n << " bytes instead of 8";
	}
}
void EventLoop::handleRead()
{
	uint64_t one = 1;
	ssize_t n = ::read(wakeupFd_, &one, sizeof one);
	if (n != sizeof one)
	{
		LOG_ERROR << "EventLoop::handleRead() reads " << n << " bytes instead of 8";
	}
}

void EventLoop::doPendingFunctors()
{
	std::vector<Functor> functors;
	callingPendingFunctors_ = true;

	{
		MutexLockGuard lock(mutex_);
		functors.swap(pendingFunctors_);
	}

	for (size_t i = 0; i < functors.size(); ++i)
	{
		functors[i]();
	}
	callingPendingFunctors_ = false;
}

void EventLoop::updateChannel(Channel *channel)
{
	assert(channel->ownerLoop() == this);
	assertInLoopThread();
	poller_->updateChannel(channel);
}

void EventLoop::runInLoop(const Functor &cb)
{
	if (isInLoopThread()) {
		cb();
	} else {
		queueInLoop(cb);
	}
}
void EventLoop::queueInLoop(const Functor &cb)
{
	{
		MutexLockGuard lock(mutex_);
		pendingFunctors_.push_back(cb);
	}

	if (!isInLoopThread() || callingPendingFunctors_)
	{
		wakeup();
	}
}


