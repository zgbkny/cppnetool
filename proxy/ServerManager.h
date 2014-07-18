#ifndef PROXY_SERVERMANAGER_H
#define PROXY_SERVERMANAGER_H
#include <cppnetool/net/TcpClient.h>
#include <cppnetool/net/InetAddress.h>
#include <cppnetool/net/EventLoop.h>
#include <string>

using namespace cppnetool;
using namespace cppnetool::net;
using std::placeholders::_1;
class ServerManager {
public:
	ServerManager(std::string ip, int port, EventLoop *loop);
	TcpClient *getTcpClient();

	bool init(int size);
private:
	void onTcpClientConnection_(TcpConnection *conn);
	InetAddress serverAddr_;
	EventLoop *loop_;

	std::queue<TcpClient *> newTcpClients_;
	std::queue<TcpClinet *> activeTcpClients_;
};

#endif /*PROXY_SERVERMANAGER_H*/