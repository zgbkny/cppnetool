#include <cppnetool/net/TcpServer.h>
#include <cppnetool/net/Acceptor.h>
#include <cppnetool/net/EventLoop.h>
#include <stdio.h>


using namespace cppnetool;
using namespace cppnetool::net;

TcpServer::TcpServer(EventLoop *loop, const InetAddress &listenAddr)
	:	loop_(loop),
		name_(listenAddr.toHostPort()),
		acceptor_(new Acceptor(loop, listenAddr)),
		started_(false),
		nextConnId_(1)
{
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
	if (acceptor_->listenning()) {
		loop_->runInLoop(
			std::bind(&Acceptor::listen, get_pointer(acceptor_)));
	}
}

void newConnection(int sockfd, const InetAddress &peerAddr)
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
	conn->connectEstablished();
}