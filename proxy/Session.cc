#include <proxy/Session.h>
#include <cppnetool/net/Callbacks.h>
#include <cppnetool/base/Logging.h>

#define TIMEOUT 6
#define CONNECT_TIMEOUT 10

Session::Session(ServerManager *serverManager, 
				 std::pair<TcpServer *, const std::string> *serverPair, 
				 TcpClient *tcpClient, std::string name)
	:	serverPair_(serverPair),
		tcpClient_(tcpClient),
		name_(name),
		serverManager_(serverManager),
		clientTime_(addTime(Timestamp::now(), 60)),
		serverTime_(clientTime_),
		connecting_(false)
{
	LOG_TRACE << "Session::Session";
	TcpConnectionPtr connPtr1 = serverPair->first->getConn(serverPair->second);
	if (connPtr1 != NULL) {
		connPtr1->setMessageCallback(
			std::bind(&Session::onTcpServerMessage_, this, _1, _2, _3));
		serverPair->first->setCloseCallback(
			std::bind(&Session::onTcpServerClose_, this, _1));
	}
	connPtr1->setPair(tcpClient);
	tcpClient_->setPair(serverPair);
	tcpClient_->setCloseCallback(
			std::bind(&Session::onTcpClientClose_, this, _1));
	tcpClient_->setConnectionCallback(
			std::bind(&Session::onTcpClientConnection_, this, _1));
	tcpClient_->setMessageCallback(
			std::bind(&Session::onTcpClientMessage_, this, _1, _2, _3));
	tcpClient_->connect();
	connecting_ = true;
	serverTime_.setNow();
	serverTime_ = addTime(clientTime_, TIMEOUT);
}

Session::~Session()
{
	LOG_TRACE << "Session::~Session";
	serverPair_->first->removeConn(serverPair_->second);
	delete serverPair_;
	delete tcpClient_;
	LOG_TRACE << "Session::~Session over";
}


// used to delete something 
void Session::onTcpClientClose_(TcpConnection *conn)
{
	LOG_TRACE << "Session::onTcpClientClose_";

}

//
void Session::onTcpServerClose_(TcpConnection *conn)
{
	LOG_TRACE << "Session::onTcpServerClose_";

}

void Session::onTcpClientConnection_(TcpConnection *conn)
{
	LOG_TRACE << "Session::onTcpClientConnection_";
	if (conn->connected()) {
		LOG_TRACE << "onConnection(): new connection [" << conn->name().c_str() << "] from " 
			 << conn->peerAddress().toHostPort().c_str();
		connecting_ = false;
		ConnPair *p = (ConnPair *) conn->getPair();
		assert(p != NULL);
		assert(p == serverPair_);
		serverTime_.setNow();
		serverTime_ = addTime(serverTime_, TIMEOUT);
		TcpConnectionPtr connPtr = p->first->getConn(p->second);
		if (connPtr != NULL) {
			conn->send(connPtr->inputBuffer().retrieveAsString());
			clientTime_.setNow();
			clientTime_ = addTime(clientTime_, TIMEOUT);
		} else {
			clientTime_.setNow();
		}
	} else {
		LOG_TRACE << "onConnection(): connection [" << conn->name().c_str() << "] is down";
	}
	
}


void Session::onTcpClientMessage_(TcpConnection *conn, Buffer *buf, Timestamp receiveTime)
{
	LOG_TRACE << "Session::onTcpClientMessage(): received" << buf->readableBytes() 
			  << "bytes from connection [" << conn->name()
			  << "] at " << receiveTime.toFormattedString();
	serverTime_.setNow();
	serverTime_ = addTime(serverTime_, TIMEOUT);
	assert(conn->getPair() != NULL);
	if (conn->getPair() != NULL) {
		LOG_TRACE << "pair not NULL";
		std::pair<TcpServer *, const std::string> *p = 
			static_cast<std::pair<TcpServer *, const std::string> *>(conn->getPair());
		TcpConnectionPtr connPtr = p->first->getConn(p->second);
		if (connPtr != NULL) {
			connPtr->send(buf->retrieveAsString());
			clientTime_.setNow();
			clientTime_ = addTime(clientTime_, TIMEOUT);
		} else { // this session need to delete(process in timeout) 
			serverTime_.setNow();
			LOG_TRACE << " Proxy::onTcpClientMessage_ client closed already ! ";
		}
	}
}


void Session::onTcpServerMessage_(TcpConnection *conn, Buffer *buf, Timestamp receiveTime)
{

	LOG_TRACE << "onTcpServerMessage(): received" << buf->readableBytes() 
			  << "bytes from connection [" << conn->name().c_str() 
			  << "] at " << receiveTime.toFormattedString().c_str();
	clientTime_.setNow();
	clientTime_ = addTime(clientTime_, TIMEOUT);
	assert(conn->getPair() != NULL);
	if (conn->getPair() != NULL) {
		TcpClient *tcpClient = (TcpClient *)conn->getPair();
		assert(tcpClient != NULL);
		assert(tcpClient == tcpClient_);
		if (tcpClient->getConn() != NULL) {
			TcpConnectionPtr connPtr = tcpClient->getConn();
			connPtr->send(buf->retrieveAsString());
			serverTime_.setNow();
			serverTime_ = addTime(serverTime_, TIMEOUT);
		} else { // need to connect
			if (connecting_) {

			} else {
				tcpClient_->connect();
				connecting_ = true;
				serverTime_.setNow();
				serverTime_ = addTime(clientTime_, TIMEOUT);
			}
		}
			
	
	}
}

std::string Session::genKey_()
{
	std::string ret = "kashgas234";
	return ret;
}

