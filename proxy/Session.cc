#include <proxy/Session.h>
#include <cppnetool/base/Logging.h>

Session::Session(std::pair<TcpServer *, const std::string> *serverPair, 
				 TcpClient *tcpClient, std::string name)
	:	serverPair_(serverPair),
		tcpClient_(tcpClient),
		name_(name)
{
	LOG_TRACE << "Session::Session";
	TcpConnectionPtr connPtr = serverPair->first->getConn(serverPair->second);
	if (connPtr != NULL) {
		connPtr->setMessageCallback(
			std::bind(&Session::onTcpServerMessage_, this, _1, _2, _3));
	}
}

void Session::onTcpServerMessage_(TcpConnection *conn, Buffer *buf, Timestamp receiveTime)
{

	LOG_TRACE << "onTcpServerMessage(): received" << buf->readableBytes() 
			  << "bytes from connection [" << conn->name().c_str() 
			  << "] at " << receiveTime.toFormattedString().c_str();
}