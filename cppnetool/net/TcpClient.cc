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
namespace detail
{


}
}
using namespace cppnetool::net;
TcpClient::TcpClient(EventLoop *loop, const InetAddress &serverAddr)
	:	loop_(loop),
		connector_(new Connector(loop, serverAddr)),
		retry_(false),
		connect_(false),
		nextConnId_(1)
{
	connector_->setNewConnectionCallback(
		std::bind(&TcpClient::newConnection, this, _1));
}
TcpClient::~TcpClient()
{

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