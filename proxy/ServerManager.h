#ifndef PROXY_SERVERMANAGER_H
#define PROXY_SERVERMANAGER_H
#include <cppnetool/net/TcpClient.h>
#include <cppnetool/net/InetAddress.h>
#include <cppnetool/net/EventLoop.h>
#include <string>

class ServerManager {
public:
	ServerManager(std::string ip, int port, EventLoop *loop);
	TcpClient *getTcpClient();
private:
	InetAddress serverAddr_;
	EventLoop *loop_;
};

#endif /*PROXY_SERVERMANAGER_H*/