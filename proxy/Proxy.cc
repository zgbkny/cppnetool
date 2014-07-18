#include <proxy/Proxy.h>
#include <utility>
#include <stdio.h>
#include <iostream>


Proxy::Proxy(uint16_t port, const std::string& ip, uint16_t servPort, int size)
	:	listenAddr_(port),
		serverAddr_(ip, servPort),
		loop_(),
		tcpServer_(&loop_, listenAddr_),
		serverManager_(ip, servPort, &loop_)
{
	serverManager_.init(size);
}

void Proxy::onTcpClientConnection_(TcpConnection *conn)
{
	if (conn->connected()) {
		LOG_TRACE << "onTcpClientConnection(): new connection [" << conn->name().c_str() << "] from " 
			 << conn->peerAddress().toHostPort().c_str();

		if (conn->getPair() != NULL) {
			LOG_DEBUG << "pair not NULL";
			std::pair<TcpServer *, const std::string> *p = 
				static_cast<std::pair<TcpServer *, const std::string> *>(conn->getPair());
			LOG_DEBUG << "pair not NULL1" << p->second;
			TcpConnectionPtr connPtr = p->first->getConn(p->second);
			LOG_DEBUG << "pair not NULL2";
			if (connPtr != NULL)
				conn->send(connPtr->inputBuffer().retrieveAsString());
			else
				LOG_DEBUG << " Proxy::onTcpClientConnection_ client closed already ! ";
		} else {
			LOG_DEBUG << "pair NULL";
		}
	} else {
		LOG_DEBUG << "onTcpClientConnection(): connection [" << conn->name().c_str() << "] is down";
	}
}

void Proxy::onTcpClientMessage_(TcpConnection *conn, Buffer *buf, Timestamp receiveTime)
{
	LOG_TRACE << "onTcpClientMessage(): received" << buf->readableBytes() 
			  << "bytes from connection [" << conn->name()
			  << "] at " << receiveTime.toFormattedString();
	if (conn->getPair() != NULL) {
		LOG_DEBUG << "pair not NULL";
		std::pair<TcpServer *, const std::string> *p = 
			static_cast<std::pair<TcpServer *, const std::string> *>(conn->getPair());
		TcpConnectionPtr connPtr = p->first->getConn(p->second);
		if (connPtr != NULL) 
			connPtr->send(buf->retrieveAsString());
		else 
			LOG_DEBUG << " Proxy::onTcpClientMessage_ client closed already ! ";
	} else {
		LOG_TRACE << "pair NULL";
	}
	
}


void Proxy::onTcpServerConnection_(TcpConnection *conn)
{
	LOG_DEBUG << "here";
	if (conn->connected()) {
		LOG_TRACE << "onTcpServerConnection(): new connection [" << conn->name().c_str() << "] from " 
			 << conn->peerAddress().toHostPort().c_str();
		std::pair<TcpServer *, const std::string> *p
			= new std::pair<TcpServer *, const std::string>(&tcpServer_, conn->name());
		TcpClient *tcpClient = serverManager_.getTcpClient();
		assert(tcpClient != NULL);
		//assert(tcpClient->getConn() != NULL);
		std::string name = "session " + conn->name();// + "->" + tcpClient->getConn()->name();
		SessionPtr sessionPtr(new Session(p, tcpClient, name));
	} else {
		LOG_TRACE << "onTcpServerConnection(): connection [" << conn->name().c_str() << "] is down";
	}
	
}
void Proxy::onTcpServerMessage_(TcpConnection *conn, Buffer *buf, Timestamp receiveTime)
{

	LOG_TRACE << "onTcpServerMessage(): received" << buf->readableBytes() 
			  << "bytes from connection [" << conn->name().c_str() 
			  << "] at " << receiveTime.toFormattedString().c_str();
/*
	//LOG_TRACE << "onTcpServerMessage(): [" << buf->retrieveAsString().c_str() << "]";
	TcpClient *tcpClient = new TcpClient(&loop_, serverAddr_);
	tcpClient->setConnectionCallback(
		std::bind(&Proxy::onTcpClientConnection_, this, _1));
	tcpClient->setMessageCallback(
		std::bind(&Proxy::onTcpClientMessage_, this, _1, _2, _3));
	std::pair<TcpServer *, const std::string> *p
		= new std::pair<TcpServer *, const std::string>(&tcpServer_, conn->name());
	tcpClient->setPair(static_cast<void *>(p));
	qTcpClient_.push(tcpClient);
	conn->setPair(tcpClient);
	tcpClient->connect();*/
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

