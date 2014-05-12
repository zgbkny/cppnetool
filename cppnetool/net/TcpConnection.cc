#include <cppnetool/net/TcpConnection.h>
#include <cppnetool/net/EventLoop.h>
#include <cppnetool/net/Channel.h>
#include <cppnetool/net/Socket.h>
#include <cppnetool/base/Logging.h>
#include <unistd.h>

using namespace cppnetool;
using namespace cppnetool::net;

TcpConnection::TcpConnection(EventLoop* loop,
                             const std::string& nameArg,
                             int sockfd,
                             const InetAddress& localAddr,
                             const InetAddress& peerAddr)
	:	loop_(loop),
		name_(nameArg),
		state_(kConnecting),
		socket_(new Socket(sockfd)),
		channel_(new Channel(loop, sockfd)),
		localAddr_(localAddr),
		peerAddr_(peerAddr)
{
	LOG_DEBUG << "TcpConnection::ctor[" <<  name_ << "] at " << this
			<< " fd=" << sockfd;
	channel_->setReadCallback(
			std::bind(&TcpConnection::handleRead, this));
}

TcpConnection::~TcpConnection()
{
	LOG_DEBUG << "TcpConnection::dtor[" <<  name_ << "] at " << this
			<< " fd=" << channel_->fd();
}

void TcpConnection::connectEstablished()
{
	loop_->assertInLoopThread();
	assert(state_ == kConnecting);
	setState(kConnected);
	channel_->enableReading();

	connectionCallback_(this);
}

void TcpConnection::connectDestroyed()
{
	loop_->assertInLoopThread();
	assert(state_ == kConnected);
	setState(kDisconnected);
	channel_->disableAll();

	// ----------------- FIX ME need this function? ---------------------
	connectionCallback_(this);

	loop_->removeChannel(channel_.get());
}

void TcpConnection::handleRead()
{
	char buf[65535];
	ssize_t n = ::read(channel_->fd(), buf, sizeof buf);
	if (n > 0) {
		messageCallback_(this, buf, n);
	} else if (n == 0) {
		handleClose();
	} else {
		handleError();
	}
}

void TcpConnection::handleWrite()
{
	
}

void TcpConnection::handleError()
{
	int err = sockets::getSocketError(channel_->fd());
	LOG_ERROR << "TcpConnection::handleError [" << name_
			  << "] - SO_ERROR = " << err << " " << strerror_tl(err);
}

void TcpConnection::handleClose()
{
	loop_->assertInLoopThread();
	LOG_TRACE << "TcpConnection::handleClose state = " << state_;
	assert(state_ == kConnected);
	// we don't close fd, leave it to dtor, so we can find leaks easily
	channel_->disableAll();
	// must be the last line 
	closeCallback_(this);
}

