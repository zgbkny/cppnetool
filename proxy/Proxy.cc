#include <proxy/Proxy.h>
#include <utility>
#include <stdio.h>
#include <iostream>




Proxy::Proxy(uint16_t port, const std::string& ip, uint16_t servPort, int size)
	:	listenAddr_(port),
		serverAddr_(ip, servPort),
		loop_()
{
	tcpServerLoop_ = &loop_;
	tcpClientLoop_ = &loop_;
	loop_.setLoopCallback(
		std::bind(&Proxy::processSession_, this));
	
	tcpServer_ = new TcpServer(&loop_, listenAddr_);
	serverManager_ = new ServerManager(ip, servPort, &loop_);
	//serverManager_->init(size);
}

Proxy::~Proxy()
{
	delete tcpServer_;
	delete serverManager_;
}

void Proxy::onTcpServerConnection_(TcpConnection *conn)
{
	LOG_DEBUG << "here";
	if (conn->connected()) {
		LOG_TRACE << "onTcpServerConnection(): new connection [" << conn->name().c_str() << "] from " 
			 << conn->peerAddress().toHostPort().c_str();
		ConnPair *p = new ConnPair(tcpServer_, conn->name());
		TcpClient *tcpClient = serverManager_->getTcpClient();
		LOG_DEBUG << "before";
		//assert(tcpClient != NULL);
		//assert(tcpClient->getConn() != NULL);
		if (tcpClient != NULL) {
			assert(tcpClient != NULL);
			
			LOG_DEBUG << "TcpClient not null";
			LOG_DEBUG << conn->name();
			std::string name = "session " + conn->name();
			SessionPtr sessionPtr(new Session(serverManager_, p, tcpClient, name));
			sessionMap_[name] = sessionPtr;
		} else {
			LOG_DEBUG << "TcpClient null";
			//delete tcpClient;
			pairs_.push(p);
			conn->disableAll();
		}
	} else {
		LOG_TRACE << "onTcpServerConnection(): connection [" << conn->name().c_str() << "] is down";
	}
	
}


void Proxy::set_conf()
{
	LOG_TRACE << "Proxy::set_conf";
	
}

bool Proxy::init() 
{
	LOG_TRACE << "Proxy::init";
	tcpServer_->setConnectionCallback(
		std::bind(&Proxy::onTcpServerConnection_, this, _1));
	tcpServer_->start();
	//tcpServerLoop_->runEvery(1, 
		//std::bind(&Proxy::processSession_, this));
	tcpServerLoop_->runEvery(5,
		std::bind(&Proxy::processQueue_, this));
	return true;
}

void Proxy::processSession_()
{
	LOG_TRACE << "Proxy::processSession_ " << sessionMap_.size();
	Timestamp now = Timestamp::now();
	SessionMap::iterator it;
	for (it = sessionMap_.begin(); it != sessionMap_.end(); it++) {
		if (it->second->getClientTime() < now || it->second->getServerTime() < now) {
			sessionMap_.erase(it->first);
		}
		LOG_TRACE << "Proxy::processSession_ check";
	}
}

void Proxy::processQueue_()
{
	LOG_TRACE << "Proxy::processQueue_";
	while (!pairs_.empty()) {
		ConnPair *p = pairs_.front();
		TcpConnectionPtr connPtr = p->first->getConn(p->second);
		pairs_.pop();
		if (connPtr != NULL) {
			assert(connPtr != NULL);
			TcpClient *tcpClient = serverManager_->getTcpClient();
			if (tcpClient != NULL) {
				connPtr->enableReading();
				connPtr->enableWriting();
				std::string name = "session " + connPtr->name();
				SessionPtr sessionPtr(new Session(serverManager_, p, tcpClient, name));
				sessionMap_[name] = sessionPtr;
			} else {
				//delete tcpClient;
				pairs_.push(p);
				break;
			}
		}
	}
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

