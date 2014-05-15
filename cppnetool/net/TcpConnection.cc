#include <cppnetool/net/TcpConnection.h>
#include <cppnetool/net/EventLoop.h>
#include <cppnetool/net/Channel.h>
#include <cppnetool/net/Socket.h>
#include <cppnetool/net/SocketOps.h>
#include <cppnetool/base/Logging.h>
#include <unistd.h>
#include <string.h>

using namespace cppnetool;
using namespace cppnetool::net;
using std::placeholders::_1;

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
		std::bind(&TcpConnection::handleRead, this, _1));
	channel_->setWriteCallback(
		std::bind(&TcpConnection::handleWrite, this));
	channel_->setCloseCallback(
		std::bind(&TcpConnection::handleClose, this));
	channel_->setErrorCallback(
		std::bind(&TcpConnection::handleError, this));
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
	assert(state_ == kConnected || state_ == kDisconnecting);
	setState(kDisconnected);
	channel_->disableAll();

	// ----------------- FIX ME need this function? ---------------------
	connectionCallback_(this);

	loop_->removeChannel(channel_.get());
}

void TcpConnection::handleRead(Timestamp receiveTime)
{
	int savedErrno;
	ssize_t n = inputBuffer_.readFd(channel_->fd(), &savedErrno);
	if (n > 0) {
		messageCallback_(this, &inputBuffer_, receiveTime);
	} else if (n == 0) {
		handleClose();
	} else {
		errno = savedErrno;
		LOG_SYSERR << "TcpConnection::handleRead";
		handleError();
	}
}


/**
 * here we dont need to deal with error 
 * cause if there is an error, we will read it in handleRead with val 0, 
 * then the connection will be removed
 */

void TcpConnection::handleWrite()
{
	loop_->assertInLoopThread();
	if (channel_->isWriting()) {
		ssize_t n = ::write(channel_->fd(),
							outputBuffer_.peek(),
							outputBuffer_.readableBytes());
		if (n > 0) {
			outputBuffer_.retrieve(n);
			if (outputBuffer_.readableBytes() == 0) {
				channel_->disableWriting();

				if (writeCompleteCallback_) {
					loop_->queueInLoop(
						std::bind(writeCompleteCallback_, this));
				}

				if (state_ == kDisconnecting) {
					shutdownInLoop();
				} else {
					LOG_TRACE << "i am going to write more data";
				}
			}
		} else {
			LOG_SYSERR << "TcpConnection::handleWrite";
		}
	} else {
		LOG_TRACE << "connection is down, no more writing";
	}
}

void TcpConnection::handleError()
{
	int err = sockets::getSocketError(channel_->fd());
	LOG_ERROR << "TcpConnection::handleError [" << name_
			  << "] - SO_ERROR = " << err << " " << strerror(err);
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

void TcpConnection::send(const std::string &message)
{
	if (state_ == kConnected) {
		if (loop_->isInLoopThread()) {
			sendInLoop(message);
		} else {
			loop_->runInLoop(
				std::bind(&TcpConnection::sendInLoop, this, message));
		}
	}
}

void TcpConnection::sendInLoop(const std::string &message)
{
	loop_->assertInLoopThread();
	ssize_t nwrote = 0;
	// if nothing in output queue, try writing directly
	if (!channel_->isWriting() && outputBuffer_.readableBytes() == 0) {
		nwrote = ::write(channel_->fd(), message.data(), message.size());
		if (nwrote >= 0) {
			if (static_cast<size_t>(nwrote) < message.size()) {
				LOG_TRACE << "i am going to write more data";
			} else if (writeCompleteCallback_) {
				loop_->queueInLoop(
					std::bind(writeCompleteCallback_, this));
			}
		} else {
			nwrote = 0;
			if (errno != EWOULDBLOCK) {
				LOG_SYSERR << "TcpConnection::sendInLoop";
			}
		}
	}

	assert(nwrote >= 0);
	if (static_cast<size_t>(nwrote) < message.size()) {
		outputBuffer_.append(message.data() + nwrote, message.size() - nwrote);
		if (!channel_->isWriting()) {
			channel_->enableWriting();
		}
	}
}

void TcpConnection::shutdown()
{
	if (state_ == kConnected) {
		setState(kDisconnecting);
		loop_->runInLoop(std::bind(&TcpConnection::shutdownInLoop, this));
	}
}

void TcpConnection::shutdownInLoop()
{
	loop_->assertInLoopThread();
	if (!channel_->isWriting()) {
		socket_->shutdownWrite();
	}
}

void TcpConnection::setTcpNoDelay(bool on)
{
	socket_->setTcpNoDelay(on);
}

void TcpConnection::setKeepAlive(bool on)
{
	socket_->setKeepAlive(on);
}