#include <proxy/Session.h>
#include <cppnetool/base/Logging.h>

Session::Session(std::pair<TcpServer *, const std::string> *serverPair, 
				 TcpClient *tcpClient, std::string name)
	:	serverPair_(serverPair),
		tcpClient_(tcpClient),
		name_(name)
{
	LOG_TRACE << "Session::Session";
	TcpConnectionPtr connPtr1 = serverPair->first->getConn(serverPair->second);
	if (connPtr1 != NULL) {
		connPtr1->setMessageCallback(
			std::bind(&Session::onTcpServerMessage_, this, _1, _2, _3));
	}
	TcpConnectionPtr connPtr2 = tcpClient->getConn();
	if (connPtr2 != NULL) {
		connPtr2->setMessageCallback(
			std::bind(&Session::onTcpClientMessage_, this, _1, _2, _3));
	}
	connPtr1->setPair(tcpClient);
	connPtr2->setPair(serverPair);
}

void Session::onTcpClientMessage_(TcpConnection *conn, Buffer *buf, Timestamp receiveTime)
{
	LOG_TRACE << "onTcpClientMessage(): received" << buf->readableBytes() 
			  << "bytes from connection [" << conn->name()
			  << "] at " << receiveTime.toFormattedString();
	if (conn->getPair() != NULL) {
		LOG_TRACE << "pair not NULL";
		std::pair<TcpServer *, const std::string> *p = 
			static_cast<std::pair<TcpServer *, const std::string> *>(conn->getPair());
		TcpConnectionPtr connPtr = p->first->getConn(p->second);
		if (connPtr != NULL) 
			connPtr->send(buf->retrieveAsString());
		else 
			LOG_TRACE << " Proxy::onTcpClientMessage_ client closed already ! ";
	} else {
		LOG_TRACE << "pair NULL";
	}
}


void Session::onTcpServerMessage_(TcpConnection *conn, Buffer *buf, Timestamp receiveTime)
{

	LOG_TRACE << "onTcpServerMessage(): received" << buf->readableBytes() 
			  << "bytes from connection [" << conn->name().c_str() 
			  << "] at " << receiveTime.toFormattedString().c_str();
	if (conn->getPair() != NULL) {
		TcpClient *tcpClient = (TcpClient *)conn->getPair();
		TcpConnectionPtr connPtr = tcpClient->getConn();
		if (connPtr != NULL) 
			connPtr->send(buf->retrieveAsString());
		else 
			LOG_TRACE << " Proxy::onTcpServerMessage_ client closed already ! ";
	} else {
		LOG_TRACE << "pair NULL";
	}
}