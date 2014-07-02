#ifndef PROXY_H
#define PROXY_H

#include <cppnetool/net/InetAddress.h>
#include <cppnetool/net/TcpServer.h>
#include <cppnetool/net/EventLoop.h>
#include <cppnetool/net/TcpConnection.h>

#include <cppnetool/net/Buffer.h>
#include <cppnetool/net/TcpClient.h>
#include <cppnetool/base/Timestamp.h>
#include <cppnetool/base/Logging.h>

#include <queue>

using namespace cppnetool::net;
using namespace cppnetool;
using std::placeholders::_1;
using std::placeholders::_2;
using std::placeholders::_3;
class Proxy {
public:
	Proxy(uint16_t port);
	void set_conf();
	void valid_conf();
	bool init();
	bool start_proxies();
	void loop();
	
	//

private:
	void onTcpServerConnection_(TcpConnection *conn);
	void onTcpServerMessage_(TcpConnection *conn, Buffer *buf, Timestamp receiveTime);
	InetAddress listenAddr_;
	EventLoop loop_;
	TcpServer tcpServer_;
	queue<TcpClient *> qTcpClient_;
};

#endif /*PROXY_H*/