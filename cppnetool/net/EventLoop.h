#ifndef CPPNETOOL_NET_EVENTLOOP_H
#define CPPNETOOL_NET_EVENTLOOP_H
#include <pthread.h>
#include <cppnetool/base/Thread.h>
#include <cppnetool/base/debug.h>
#include <cppnetool/base/Mutex.h>
#include <cppnetool/net/Callbacks.h>
#include <cppnetool/net/TimerId.h>
#include <assert.h>
#include <vector>
#include <memory>

namespace cppnetool
{
namespace net
{

class Channel;
class Poller;
class TimerQueue;

class EventLoop
{
 public:
 	typedef std::function<void()> Functor;
 	EventLoop();
 	~EventLoop();

 	void loop();
 	void quit();

 	Timestamp pollReturnTime() const { return pollReturnTime_; }

	void runInLoop(const Functor &cb);
	void queueInLoop(const Functor &cb);

	TimerId runAt(const Timestamp& time, const TimerCallback& cb);
	///
	/// Runs callback after @c delay seconds.
	/// Safe to call from other threads.
	///
	TimerId runAfter(double delay, const TimerCallback& cb);
	///
	/// Runs callback every @c interval seconds.
	/// Safe to call from other threads.
	///
	TimerId runEvery(double interval, const TimerCallback& cb);

	void cancel(TimerId timerId);

	// internal use only
	void wakeup();
	void updateChannel(Channel *channel);
	void removeChannel(Channel *channel);

 	void assertInLoopThread()
 	{
 		if (!isInLoopThread())
 		{
 			abortNotInLoopThread();
 		}
 	}
 	bool isInLoopThread() const { return threadId_ == CurrentThread::tid();}

 	
 	
 private:
 	void abortNotInLoopThread();
 	void handleRead();  // waked up
	void doPendingFunctors();

 	typedef std::vector<Channel *> ChannelList;

 	bool looping_; /* atomic */
 	bool quit_; /* atomic */
 	bool callingPendingFunctors_; /* atomic */
 	const pid_t threadId_;
 	Timestamp pollReturnTime_;
 	std::unique_ptr<Poller> poller_;
 	std::shared_ptr<TimerQueue> timerQueue_;
 	int wakeupFd_;
 	std::shared_ptr<Channel> wakeupChannel_;
 	ChannelList activeChannels_;
 	MutexLock mutex_;
	std::vector<Functor> pendingFunctors_; // @Buarded By mutex_
};
}/*end namespace net*/
}/*end namespace cppnetool*/

#endif /*CPPNETOOL_NET_EVENTLOOP_H*/
