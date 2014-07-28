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
		nextConnId_(1),
		pair_(NULL)
{
	connector_->setNewConnectionCallback(
		std::bind(&TcpClient::newConnection, this, _1));
}
TcpClient::~TcpClient()
{
	LOG_INFO << "TcpClient::~TcpClient [" << this
			 << "] - connector " << connector_.get();
	TcpConnectionPtr conn;
	LOG_TRACE << "check";
	{
		MutexLockGuard lock(mutex_);
		LOG_TRACE << "check1";
		//conn = connection_;
	}
	if (state_) {
		LOG_TRACE << "not null";
		/*CloseCallback cb = std::bind(&detail::removeConnection, loop_, _1);
		loop_->runInLoop(
			std::bind(&TcpConnection::setCloseCallback, conn, cb));*/
		connection_->shutdownAll();
		connection_->connectDestroyed();
	} else {
		LOG_TRACE << "null";
		
		//connector_->stop();
	    // FIXME: HACK
		//loop_->runAfter(1, std::bind(&detail::removeConnector, connector_));
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
	if (pair_ != NULL) {
		conn->setPair(pair_);
	}
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
	LOG_DEBUG << "TcpClient::removeConnection";
	if (closeCallback_) {
		closeCallback_(conn);
	}

	loop_->assertInLoopThread();
	assert(loop_ == conn->getLoop());
	connection_->connectDestroyed();

	{
		MutexLockGuard lock(mutex_);
	    assert(connection_.get() == conn);
	    state_ = false;
		connection_.reset();
		LOG_DEBUG << "TcpClient::removeConnection" << "check";
	}

	//loop_->queueInLoop(
	//	std::bind(&TcpConnection::connectDestroyed, conn));
	
	if (retry_ && connect_) {
		LOG_INFO << "TcpClient::connect[" << this << "] - Reconnecting to "
					<< connector_->serverAddress().toHostPort();
		connector_->restart();
	}
}

TcpConnectionPtr TcpClient::getConn() {

	if (state_) {
		LOG_TRACE << "loop_:" << loop_;
		LOG_TRACE << "getLoop():" << connection_->getLoop();
		assert(loop_ != NULL);
		assert(connection_->getLoop() != NULL);
		assert(loop_ == connection_->getLoop());
		return connection_;
	} else return NULL;
}