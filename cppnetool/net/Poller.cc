#include <cppnetool/net/Poller.h>
#include <poll.h>

using namespace cppnetool;
using namespace cppnetool::net;

Poller::Poller(EventLoop *loop)
	: ownerLoop_(loop)
{
}

Poller::~Poller()
{

}

Timestamp Poller::poll(int timeoutMs, ChannelList *activeChannels)
{
	int numEvents = ::poll(&*pollfds_.begin(), pollfds_.size(), timeoutMs);
	Timestamp now(Timestamp::now());
	if (numEvents > 0) {
		fillActiveChannels(numEvents, activeChannels);
	} else if (numEvents == 0) {

	} else {

	}
	return now;
}

void Poller::fillActiveChannels(int numEvents, ChannelList *activeChannels) const
{
	for (PollFdList::const_iterator pfd = pollfds_.begin();
			pfd != pollfds_.end() && numEvents > 0; ++pfd) {
		
	}
}