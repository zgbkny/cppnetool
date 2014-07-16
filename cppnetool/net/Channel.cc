#include <poll.h>
#include <cppnetool/net/Channel.h>
#include <cppnetool/net/EventLoop.h>
#include <cppnetool/base/Logging.h>
#include <cppnetool/base/debug.h>

using namespace cppnetool::net;

const int Channel::kNoneEvent = 0;
const int Channel::kReadEvent = POLLIN | POLLPRI;
const int Channel::kWriteEvent = POLLOUT;

Channel::Channel(EventLoop *loop, int fdArg)
	:	loop_(loop), 
		fd_(fdArg), 
		events_(0), 
		revents_(0), 
		index_(-1),
		eventHandling_(false)
{
}
Channel::~Channel()
{
	LOG_DEBUG << "Channel::~Channel" << this->fd_;
	//assert(!eventHandling_);
}
void Channel::update()
{
	loop_->updateChannel(this);
}
void Channel::handleEvent(Timestamp receiveTime)
{
	eventHandling_ = true;
	LOG_DEBUG << "Channel::handleEvent" << this->fd_;
	if (revents_ & POLLNVAL) {
		LOG_WARN << "Channel::handle_event() POLLNVAL";
	}

	if ((revents_ & POLLHUP) && !(revents_ & POLLIN)) {
		LOG_WARN << "Channel::handle_event() POLLHUP";
		if (closeCallback_) {
			eventHandling_ = false;
			closeCallback_();
		}
	}

	if (revents_ & (POLLERR | POLLNVAL)) {
		LOG_WARN << "Channel::handle_event() ERROR";
		if (errorCallback_) {
			eventHandling_ = false;
			errorCallback_();
		}
	}

	if (revents_ & (POLLIN | POLLPRI | POLLRDHUP)) {
		LOG_WARN << "Channel::handle_event() READ";
		if (readCallback_) readCallback_(receiveTime); 
	}

	if (revents_ & POLLOUT) {
		LOG_WARN << "Channel::handle_event() WRITE";
		if (writeCallback_) writeCallback_();
	}
	eventHandling_ = false;
}
