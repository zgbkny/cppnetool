#include <cppnetool/base/debug.h>
#include <cppnetool/net/Poller.h>
#include <cppnetool/net/Channel.h>
#include <cppnetool/base/Logging.h>
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
		if (pfd->revents > 0)
		{
			--numEvents;
			ChannelMap::const_iterator ch = channels_.find(pfd->fd);
			assert(ch != channels_.end());
			Channel *channel = ch->second;
			assert(channel->fd() == pfd->fd);
			channel->set_revents(pfd->revents);
			activeChannels->push_back(channel);
		}
	}
}

void Poller::updateChannel(Channel *channel)
{
	LOG_TRACE << "Poller::updateChannel " << channel->index();
	assertInLoopThread();
	if (channel->index() < 0) {
		assert(channels_.find(channel->fd()) == channels_.end());
		struct pollfd pfd;
		pfd.fd = channel->fd();
		pfd.events = static_cast<short>(channel->events());
		pfd.revents = 0;
		pollfds_.push_back(pfd);
		int idx = static_cast<int>(pollfds_.size()) - 1;
		channel->set_index(idx);
		channels_[pfd.fd] = channel;
		LOG_TRACE << "Poller::updateChannel event:" << pfd.events << " fd:" << pfd.fd;
	} else {
		assert(channels_.find(channel->fd()) != channels_.end());
		assert(channels_[channel->fd()] == channel);
		int idx = channel->index();
		assert(0 <= idx && idx < static_cast<int>(pollfds_.size()));
		struct pollfd &pfd = pollfds_[idx];
		assert(pfd.fd == channel->fd() || pfd.fd == - channel->fd() - 1);
		pfd.events = static_cast<short>(channel->events());
		pfd.revents = 0;
		if (channel->isNoneEvent()) {
			pfd.fd = - channel->fd() - 1;
		}
		LOG_TRACE << "Poller::updateChannel event:" << pfd.events << " fd:" << pfd.fd;
	}
}

void Poller::removeChannel(Channel *channel)
{
	LOG_TRACE << "Poller::removeChannel " << channel->index();
	assertInLoopThread();
	assert(channels_.find(channel->fd()) != channels_.end());
	assert(channels_[channel->fd()] == channel);
	assert(channel->isNoneEvent());
	int idx = channel->index();
	LOG_TRACE << "fd=" << channel->fd();
	assert(0 <= idx && idx < static_cast<int>(pollfds_.size()));
	const struct pollfd &pfd = pollfds_[idx]; (void)pfd;
	assert(pfd.fd == - channel->fd() - 1 && pfd.events == channel->events());
	size_t n = channels_.erase(channel->fd());
	assert(n == 1); (void)n;
	if (static_cast<size_t>(idx) == pollfds_.size() - 1) {
		pollfds_.pop_back();
	} else {
		int channelAtEnd = pollfds_.back().fd;
		iter_swap(pollfds_.begin() + idx, pollfds_.end() - 1);
		if (channelAtEnd < 0) {
			channelAtEnd = - channelAtEnd - 1;
		}
		channels_[channelAtEnd]->set_index(idx);
		pollfds_.pop_back();
	}
}