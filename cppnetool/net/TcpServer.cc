#include <cppnetool/net/TcpServer.h>
#include <cppnetool/net/Acceptor.h>
#include <cppnetool/net/EventLoop.h>
#include <cppnetool/net/SocketOps.h>
#include <cppnetool/net/EventLoopThreadPool.h>
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
		threadPool_(new EventLoopThreadPool(loop)),
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

void TcpServer::setThreadNum(int numThreads)
{
	assert(0 <= numThreads);
	threadPool_->setThreadNum(numThreads);
}

void TcpServer::start()
{
	if (!started_) {
		started_ = true;
		threadPool_->start();
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
	EventLoop *ioLoop = threadPool_->getNextLoop();
	// FIXME poll with zero timeout to double confirm the new connection
	TcpConnectionPtr conn(
			new TcpConnection(ioLoop, connName, sockfd, localAddr, peerAddr));
	connections_[connName] = conn;
	conn->setConnectionCallback(connectionCallback_);
	conn->setMessageCallback(messageCallback_);
	conn->setWriteCompleteCallback(writeCompleteCallback_);
	conn->setCloseCallback(
		std::bind(&TcpServer::removeConnection, this, _1));
	
	ioLoop->runInLoop(
		std::bind(&TcpConnection::connectEstablished, conn));
}

void TcpServer::removeConnection(TcpConnection *conn)
{
	loop_->runInLoop(
		std::bind(&TcpServer::removeConnectionInLoop, this, conn));
}

void TcpServer::removeConnectionInLoop(TcpConnection *conn)
{
	loop_->assertInLoopThread();
	LOG_INFO << "TcpServer::removeConnection [" << name_
		     << "] - connection " << conn->name();
	conn->connectDestroyed();
	size_t n = connections_.erase(conn->name());
	assert(n == 1); (void)n;
	// FIXME bug here, because if a shared_ptr is erased from map, it will be destroyed, how to call its function
	/*EventLoop *ioLoop = conn->getLoop();
	ioLoop->queueInLoop(
		std::bind(&TcpConnection::connectDestroyed, conn));*/
}
