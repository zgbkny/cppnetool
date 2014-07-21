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

#include <proxy/Session.h>
#include <proxy/ServerManager.h>

#include <string>

#include <queue>

using namespace cppnetool::net;
using namespace cppnetool;
using std::placeholders::_1;
using std::placeholders::_2;
using std::placeholders::_3;
class Proxy {
public:
	Proxy(uint16_t port, const std::string& ip, uint16_t servPort, int size);
	void set_conf();
	void valid_conf();
	bool init();
	bool start_proxies();
	void loop();
	
	//

private:
	void onTcpClientConnection_(TcpConnection *conn);
	void onTcpClientMessage_(TcpConnection *conn, Buffer *buf, Timestamp receiveTime);

	void onTcpServerConnection_(TcpConnection *conn);
	void onTcpServerMessage_(TcpConnection *conn, Buffer *buf, Timestamp receiveTime);
	InetAddress listenAddr_;
	InetAddress serverAddr_;

	EventLoop loop_;
	TcpServer tcpServer_;
	ServerManager serverManager_;
	
	typedef std::shared_ptr<Session> SessionPtr;
	typedef std::map<std::string, SessionPtr> SessionMap;
	SessionMap sessionMap_;
};

#endif /*PROXY_H*/