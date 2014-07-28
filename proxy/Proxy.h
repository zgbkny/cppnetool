#ifndef PROXY_H
#define PROXY_H

#include <cppnetool/net/InetAddress.h>
#include <cppnetool/net/TcpServer.h>
#include <cppnetool/net/EventLoop.h>
#include <cppnetool/net/TcpConnection.h>
#include <cppnetool/net/EventLoopThreadPool.h>

#include <cppnetool/net/Buffer.h>
#include <cppnetool/net/TcpClient.h>
#include <cppnetool/base/Timestamp.h>
#include <cppnetool/base/Logging.h>


#include <proxy/Session.h>
#include <proxy/ServerManager.h>

#include <string>
#include <map>
#include <queue>

using namespace cppnetool::net;
using namespace cppnetool;
using std::placeholders::_1;
using std::placeholders::_2;
using std::placeholders::_3;
class Proxy {
public:
	Proxy(uint16_t port, const std::string& ip, uint16_t servPort, int size);
	~Proxy();
	void set_conf();
	void valid_conf();
	bool init();
	bool start_proxies();
	void loop();
	
	//

private:
	void onTcpServerConnection_(TcpConnection *conn);

	void processSession_();
	void processQueue_();
	InetAddress listenAddr_;
	InetAddress serverAddr_;

	EventLoop loop_;
	EventLoop *tcpServerLoop_;
	EventLoop *tcpClientLoop_;
	TcpServer *tcpServer_;
	ServerManager *serverManager_;
	
	typedef std::shared_ptr<Session> SessionPtr;
	typedef std::map<std::string, SessionPtr> SessionMap;
	typedef std::pair<TcpServer *, const std::string> ConnPair;
	SessionMap sessionMap_;
	std::queue<ConnPair *> pairs_;
};

#endif /*PROXY_H*/