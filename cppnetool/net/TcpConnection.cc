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

	connectionCallback_(std::make_shared<TcpConnection>(this));
}

void TcpConnection::handleRead()
{
	char buf[65536];
	ssize_t n = ::read(channel_->fd(), buf, sizeof buf);
	messageCallback_(std::make_shared<TcpConnection>(this), buf, n);
	// FIXME: close connection if n == 0
}

