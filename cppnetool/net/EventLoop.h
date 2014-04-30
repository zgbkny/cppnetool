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
 private:
 	void abortNotInLoopThread();
 	bool looping_; /* atomic */
 	const pid_t threadId_;
};
}/*end namespace net*/
}/*end namespace cppnetool*/

#endif /*CPPNETOOL_NET_EVENTLOOP_H*/
