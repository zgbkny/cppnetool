#include <proxy/Proxy.h>
#include <utility>
#include <stdio.h>
#include <iostream>


Proxy::Proxy(uint16_t port, const std::string& ip, uint16_t servPort)
	:	listenAddr_(port),
		serverAddr_(ip, servPort),
		loop_(),
		tcpServer_(&loop_, listenAddr_)
{

}

void Proxy::onTcpClientConnection_(TcpConnection *conn)
{
	if (conn->connected()) {
		LOG_INFO << "onTcpClientConnection(): new connection [" << conn->name().c_str() << "] from " 
			 << conn->peerAddress().toHostPort().c_str();

		if (conn->getPair() != NULL) {
			std::pair<TcpServer *, const std::string&> *p = 
				static_cast<std::pair<TcpServer *, const std::string&> *>(conn->getPair());
			TcpConnectionPtr connPtr = p->first->getConn(p->second);
			conn->send(connPtr->inputBuffer().retrieveAsString());
		} else {
			LOG_INFO << "pair NULL";
		}
	} else {
		LOG_INFO << "onTcpClientConnection(): connection [" << conn->name().c_str() << "] is down";
	}
}

void Proxy::onTcpClientMessage_(TcpConnection *conn, Buffer *buf, Timestamp receiveTime)
{
	LOG_INFO << "onTcpClientMessage(): received" << buf->readableBytes() 
			  << "bytes from connection [" << conn->name().c_str() 
			  << "] at " << receiveTime.toFormattedString().c_str();
	if (conn->getPair() != NULL) {
		std::pair<TcpServer *, const std::string&> *p = 
			static_cast<std::pair<TcpServer *, const std::string&> *>(conn->getPair());
		TcpConnectionPtr connPtr = p->first->getConn(p->second);
		connPtr->send(buf->retrieveAsString());
	} else {
		LOG_INFO << "pair NULL";
	}
	
}


void Proxy::onTcpServerConnection_(TcpConnection *conn)
{
	if (conn->connected()) {
		LOG_INFO << "onTcpServerConnection(): new connection [" << conn->name().c_str() << "] from " 
			 << conn->peerAddress().toHostPort().c_str();
	} else {
		LOG_INFO << "onTcpServerConnection(): connection [" << conn->name().c_str() << "] is down";
	}
	
}
void Proxy::onTcpServerMessage_(TcpConnection *conn, Buffer *buf, Timestamp receiveTime)
{
	LOG_INFO << "onTcpServerMessage(): received" << buf->readableBytes() 
			  << "bytes from connection [" << conn->name().c_str() 
			  << "] at " << receiveTime.toFormattedString().c_str();

	//LOG_INFO << "onTcpServerMessage(): [" << buf->retrieveAsString().c_str() << "]";
	TcpClient *tcpClient = new TcpClient(&loop_, serverAddr_);
	tcpClient->setConnectionCallback(
		std::bind(&Proxy::onTcpClientConnection_, this, _1));
	tcpClient->setMessageCallback(
		std::bind(&Proxy::onTcpClientMessage_, this, _1, _2, _3));
	std::pair<TcpServer *, const std::string&> *p
		= new std::pair<TcpServer *, const std::string&>(&tcpServer_, conn->name());
	tcpClient->setPair(static_cast<void *>(p));
	qTcpClient_.push(tcpClient);
	conn->setPair(tcpClient);
	tcpClient->connect();
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
	loop_.loop();
}

