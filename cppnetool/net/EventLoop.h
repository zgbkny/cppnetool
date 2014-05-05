#ifndef CPPNETOOL_NET_EVENTLOOP_H
#define CPPNETOOL_NET_EVENTLOOP_H
#include <pthread.h>
#include <cppnetool/base/Thread.h>
#include <assert.h>

namespace cppnetool
{
namespace net
{

class Channel;


class EventLoop
{
 public:
 	EventLoop();
 	~EventLoop();

 	void loop();
 	void assertInLoopThread()
 	{
 		if (!isInLoopThread())
 		{
 			abortNotInLoopThread();
 		}
 	}
 	bool isInLoopThread() const {return threadId_ == CurrentThread::tid();}

 	void updateChannel(Channel *channel);
 	void quit();
 private:
 	void abortNotInLoopThread();

 	typedef std::vector<Channel *> ChannelList;
 	bool looping_; /* atomic */
 	bool quit_; /* atomic */
 	const pid_t threadId_;
 	std::scoped_ptr<Poller> poller_;
 	ChannelList activeChannels_;
};
}/*end namespace net*/
}/*end namespace cppnetool*/

#endif /*CPPNETOOL_NET_EVENTLOOP_H*/
