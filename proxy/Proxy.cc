#include <proxy/Proxy.h>

#include <stdio.h>
#include <iostream>


Proxy::Proxy(uint16_t port)
	:	listenAddr_(port),
		loop_(),
		tcpServer_(&loop_, listenAddr_)
{

}

void Proxy::onTcpServerConnection_(TcpConnection *conn)
{
	if (conn->connected()) {
		LOG_TRACE << "onConnection(): new connection [" << conn->name().c_str() << "] from " 
			 << conn->peerAddress().toHostPort().c_str() << std::endl;
	} else {
		LOG_TRACE << "onConnection(): connection [" << conn->name().c_str() << "] is down" << std::endl;
	}
}
void Proxy::onTcpServerMessage_(TcpConnection *conn, Buffer *buf, Timestamp receiveTime)
{
	printf("onMessage(): received %zd bytes from connection [%s] at %s\n",
			buf->readableBytes(),
			conn->name().c_str(),
			receiveTime.toFormattedString().c_str());

	printf("onMessage(): [%s]\n", buf->retrieveAsString().c_str());
}

void Proxy::set_conf()
{
	LOG_TRACE << "Proxy::set_conf";
	
}

bool Proxy::init() 
{
	LOG_TRACE << "Proxy::init";
	tcpServer_.setConnectionCallback(
		std::bind(&Proxy::onTcpServerConnection_, this, _1));
	tcpServer_.setMessageCallback(
		std::bind(&Proxy::onTcpServerMessage_, this, _1, _2, _3));
	tcpServer_.start();
	return true;
}

bool Proxy::start_proxies()
{
	LOG_TRACE << "Proxy::start_proxies";
	return true;
}

void Proxy::loop()
{
	LOG_TRACE << qTcpClient_.size();
	qTcpClient_.push(NULL);
	LOG_TRACE << qTcpClient_.size();
	loop_.loop();
}

