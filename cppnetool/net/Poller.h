#ifndef CPPNETOOL_NET_POLLER_H
#define CPPNETOOL_NET_POLLER_H

#include <cppnetool/base/debug.h>
#include <cppnetool/net/EventLoop.h>
#include <cppnetool/base/Timestamp.h>
#include <map>
#include <vector>

struct pollfd;

namespace cppnetool 
{
namespace net
{
class Channel;
//class EventLoop;

class Poller
{
public:
	typedef std::vector<Channel *> ChannelList;

	Poller(EventLoop *loop);
	~Poller();

	/// Polls the IO events
	/// Must be called in the loop thread
	Timestamp poll(int timeoutMs, ChannelList *activeChannels);


	// change the interested IO events
	// Must be called in the loop thread
	void updateChannel(Channel *channel);

	void assertInLoopThread() { ownerLoop_->assertInLoopThread(); }

private:
	void fillActiveChannels(int numEvents, ChannelList *activeChannels) const;

	typedef std::vector<struct pollfd> PollFdList;
	typedef std::map<int, Channel *> ChannelMap;

	EventLoop *ownerLoop_;
	PollFdList pollfds_;
	ChannelMap channels_;
};

}
}



#endif /*CPPNETOOL_NET_POLLER_H*/