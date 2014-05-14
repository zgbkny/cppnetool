#include <cppnetool/net/TcpServer.h>
#include <cppnetool/net/Acceptor.h>
#include <cppnetool/net/EventLoop.h>
#include <cppnetool/net/SocketOps.h>
#include <cppnetool/base/Logging.h>
#include <stdio.h>


using namespace cppnetool;
using namespace cppnetool::net;
using std::placeholders::_1;
using std::placeholders::_2;

TcpServer::TcpServer(EventLoop *loop, const InetAddress &listenAddr)
	:	loop_(loop),
		name_(listenAddr.toHostPort()),
		acceptor_(new Acceptor(loop, listenAddr)),
		started_(false),
		nextConnId_(1)
{
//	LOG_INFO << "FIXME::TcpServer";
	acceptor_->setNewConnectionCallback(
		std::bind(&TcpServer::newConnection, this, _1, _2));
}

TcpServer::~TcpServer()
{

}

void TcpServer::start()
{
	if (!started_) {
		started_ = true;
	}
	if (!acceptor_->listenning()) {
		loop_->runInLoop(
			std::bind(&Acceptor::listen, acceptor_.get()));
	}
}

void TcpServer::newConnection(int sockfd, const InetAddress &peerAddr)
{
	loop_->assertInLoopThread();
	char buf[32];
	snprintf(buf, sizeof buf, "#%d", nextConnId_);
	++nextConnId_;
	std::string connName = name_ + buf;

	LOG_INFO << "TcpServer::newConnection [" << name_
		<< "] - new connection [" << connName
		<< "] from " << peerAddr.toHostPort();
	InetAddress localAddr(sockets::getLocalAddr(sockfd));
	// FIXME poll with zero timeout to double confirm the new connection
	TcpConnectionPtr conn(
			new TcpConnection(loop_, connName, sockfd, localAddr, peerAddr));
	connections_[connName] = conn;
	conn->setConnectionCallback(connectionCallback_);
	conn->setMessageCallback(messageCallback_);
	conn->setCloseCallback(
		std::bind(&TcpServer::removeConnection, this, _1));
	conn->connectEstablished();
}

void TcpServer::removeConnection(TcpConnection *conn)
{
	loop_->assertInLoopThread();
	LOG_INFO << "TcpServer::removeConnection [" << name_
		     << "] - connection " << conn->name();
	size_t n = connections_.erase(conn->name());
	assert(n == 1); (void)n;
	loop_->queueInLoop(
		std::bind(&TcpConnection::connectDestroyed, conn));
}