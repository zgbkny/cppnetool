#include <proxy/ServerManager.h>

ServerManager::ServerManager(std::string ip, int port, EventLoop *loop)
	:	serverAddr_(ip, port),
		loop_(loop)
{

}

TcpClient *ServerManager::getTcpClient()
{
	TcpClient *tcpClient = new TcpClient(loop_, serverAddr_);
	return tcpClient;
}