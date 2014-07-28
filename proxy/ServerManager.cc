#include <proxy/ServerManager.h>
#include <cppnetool/base/Logging.h>

ServerManager::ServerManager(std::string ip, int port, EventLoop *loop)
	:	serverAddr_(ip, port),
		loop_(loop)
{
	//loop_->runEvery(3, std::bind(&ServerManager::processRequest, this));
}

void ServerManager::processRequest()
{
	LOG_TRACE << "ServerManager::processRequest";

	BufferMap::iterator it;
    for(it = requestMap_.begin(); it != requestMap_.end(); ++it) {
    	LOG_TRACE << "key: " << it->first;
    	TcpClient *tcpClient = getTcpClient();
    	TcpConnectionPtr conn = tcpClient->getConn();
    	std::string *strP = new std::string(it->first);
    	conn->setPair(strP);
    	conn->setMessageCallback(
    		std::bind(&ServerManager::onTcpClientMessage_, this, _1, _2, _3));
    	conn->send(it->second->retrieveAsString());
    }
}


void ServerManager::onTcpClientConnection_(TcpConnection *conn)
{
	TcpClient *tcpClient = (TcpClient *)conn->getPair();
	activeTcpClients_.push(tcpClient);
}

void ServerManager::onTcpClientMessage_(TcpConnection *conn, Buffer *buf, Timestamp receiveTime)
{
	LOG_TRACE << "ServerManager::onTcpClientMessage_";
	std::string *keyP = (std::string *)conn->getPair();
	LOG_TRACE << *keyP;
	BufferPtr bufferPtr(new Buffer());
	bufferPtr->append(buf->retrieveAsString());
	responseMap_[*keyP] = bufferPtr;
	requestMap_.erase(*keyP);
}

bool ServerManager::init(int size)
{
	for (int i = 0; i < size; i++) {
		TcpClient *tcpClient = new TcpClient(loop_, serverAddr_);
		tcpClient->setConnectionCallback(
			std::bind(&ServerManager::onTcpClientConnection_, this, _1));
		tcpClient->setPair(tcpClient);
		//newTcpClients_.push(tcpClient);
		tcpClient->connect();
	}
}

TcpClient *ServerManager::getTcpClient()
{
	TcpClient *tcpClient = new TcpClient(loop_, serverAddr_);
	/*if (!activeTcpClients_.empty()) {
		tcpClient = activeTcpClients_.front();
		activeTcpClients_.pop();
	}
	if (activeTcpClients_.size() < 10) {
		for (int i = 0; i < 10; i++) {
			TcpClient *tcpClient = new TcpClient(loop_, serverAddr_);
			tcpClient->setConnectionCallback(
				std::bind(&ServerManager::onTcpClientConnection_, this, _1));
			tcpClient->setPair(tcpClient);
			//newTcpClients_.push(tcpClient);
			tcpClient->connect();
		}
	}*/
	return tcpClient;
}