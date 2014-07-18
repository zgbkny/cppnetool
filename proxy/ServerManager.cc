#include <proxy/ServerManager.h>

ServerManager::ServerManager(std::string ip, int port, EventLoop *loop)
	:	serverAddr_(ip, port),
		loop_(loop)
{

}

void ServerManager::onTcpClientConnection_(TcpConnection *conn)
{

}

bool ServerManager::init(int size)
{
	for (int i = 0; i < size; i++) {
		TcpClient *tcpClient = new TcpClient(loop_, serverAddr_);
		tcpClient->setConnectionCallback(
			std::bind(&ServerManager::onTcpClientConnection_, this, _1));
		tcpClient->setPair(tcpClient);
		newTcpClients_.add(tcpClient);
		tcpClient_.connect();
	}
}

TcpClient *ServerManager::getTcpClient()
{
	TcpClient *tcpClient = new TcpClient(loop_, serverAddr_);
	return tcpClient;
}