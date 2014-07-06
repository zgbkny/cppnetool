#include <cppnetool/net/TcpClient.h>
#include <cppnetool/net/EventLoop.h>
#include <cppnetool/net/Connector.h>
#include <cppnetool/net/SocketOps.h>
#include <cppnetool/base/Logging.h>
#include <cppnetool/net/TcpConnection.h>
#include <string>

using namespace cppnetool;
using std::placeholders::_1;
using std::string;
namespace cppnetool
{
namespace net
{
namespace detail
{
void removeConnection(EventLoop* loop, TcpConnection *conn)
{
	loop->queueInLoop(std::bind(&TcpConnection::connectDestroyed, conn));
}

void removeConnector(const ConnectorPtr& connector)
{
  //connector->
}
}
}
}
using namespace cppnetool::net;
TcpClient::TcpClient(EventLoop *loop, const InetAddress &serverAddr)
	:	loop_(loop),
		connector_(new Connector(loop, serverAddr)),
		retry_(false),
		connect_(false),
		state_(false),
		nextConnId_(1)
{
	connector_->setNewConnectionCallback(
		std::bind(&TcpClient::newConnection, this, _1));
}
TcpClient::~TcpClient()
{
	LOG_INFO << "TcpClient::~TcpClient [" << this
			 << "] - connector " << connector_.get();
	TcpConnectionPtr conn;
	{
		MutexLockGuard lock(mutex_);
		conn = connection_;
	}
	if (conn) {
		CloseCallback cb = std::bind(&detail::removeConnection, loop_, _1);
		loop_->runInLoop(
			std::bind(&TcpConnection::setCloseCallback, conn, cb));
	} else {
		connector_->stop();
	    // FIXME: HACK
		loop_->runAfter(1, std::bind(&detail::removeConnector, connector_));
	}
}
void TcpClient::connect()
{
	connect_ = true;
	connector_->start();
}

void TcpClient::disconnect()
{
	connect_ = false;

	{
		MutexLockGuard lock(mutex_);
		if (connection_) {
			connection_->shutdown();
		}
	}
}

void TcpClient::stop()
{
	connect_ = false;
	connector_->stop();
}

void TcpClient::newConnection(int sockfd)
{
	state_ = true;
	loop_->assertInLoopThread();
	InetAddress peerAddr(sockets::getPeerAddr(sockfd));
	char buf[32];
	snprintf(buf, sizeof buf, ":%s#%d", peerAddr.toHostPort().c_str(), nextConnId_);
	++nextConnId_;
	string connName = buf;

	InetAddress localAddr(sockets::getLocalAddr(sockfd));
	// FIXME poll with zero timeout to double confirm the new connection
	// FIXME use make_shared if necessary
	TcpConnectionPtr conn(new TcpConnection(loop_,
											connName,
											sockfd,
											localAddr,
											peerAddr));

	conn->setConnectionCallback(connectionCallback_);
	conn->setMessageCallback(messageCallback_);
	conn->setWriteCompleteCallback(writeCompleteCallback_);
	conn->setCloseCallback(
			std::bind(&TcpClient::removeConnection, this, _1)); // FIXME: unsafe
	{
		MutexLockGuard lock(mutex_);
		connection_ = conn;
	}
	conn->connectEstablished();
}

void TcpClient::removeConnection(TcpConnection *conn)
{
	state_ = false;
	loop_->assertInLoopThread();
	assert(loop_ == conn->getLoop());

	{
		MutexLockGuard lock(mutex_);
	    assert(connection_.get() == conn);
		connection_.reset();
	}

	loop_->queueInLoop(
		std::bind(&TcpConnection::connectDestroyed, conn));
	if (retry_ && connect_) {
		LOG_INFO << "TcpClient::connect[" << this << "] - Reconnecting to "
					<< connector_->serverAddress().toHostPort();
		connector_->restart();
	}
}