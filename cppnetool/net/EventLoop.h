#ifndef CPPNETOOL_NET_EVENTLOOP_H
#define CPPNETOOL_NET_EVENTLOOP_H
#include <pthread.h>
#include <cppnetool/base/Thread.h>
#include <cppnetool/base/debug.h>
#include <cppnetool/base/Mutex.h>
//#include <cppnetool/net/Poller.h>
//#include <cppnetool/net/Channel.h>
#include <assert.h>
#include <vector>
#include <memory>

namespace cppnetool
{
namespace net
{

class Channel;
class Poller;

class EventLoop
{
 public:
 	typedef std::function<void()> Functor;
 	EventLoop();
 	~EventLoop();

 	void loop();
 	void quit();

	void runInLoop(const Functor &cb);
	void queueInLoop(const Functor &cb);


 	void assertInLoopThread()
 	{
 		if (!isInLoopThread())
 		{
 			abortNotInLoopThread();
 		}
 	}
 	bool isInLoopThread() const { return threadId_ == CurrentThread::tid();}

 	// internal use only
	void wakeup();
 	void updateChannel(Channel *channel);
 	
 private:
 	void abortNotInLoopThread();
 	void handleRead();  // waked up
	void doPendingFunctors();

 	typedef std::vector<Channel *> ChannelList;
 	bool looping_; /* atomic */
 	bool quit_; /* atomic */
 	bool callingPendingFunctors_; /* atomic */
 	const pid_t threadId_;

 	std::unique_ptr<Poller> poller_;

 	int wakeupFd_;
 	std::unique_ptr<Channel> wakeupChannel_;
 	ChannelList activeChannels_;
 	MutexLock mutex_;
	std::vector<Functor> pendingFunctors_; // @Buarded By mutex_
};
}/*end namespace net*/
}/*end namespace cppnetool*/

#endif /*CPPNETOOL_NET_EVENTLOOP_H*/
