#ifndef SESSION_H
#define SESSION_H

#include <cppnetool/net/TcpServer.h>
#include <cppnetool/net/TcpClient.h>
#include <cppnetool/net/TcpConnection.h>
#include <cppnetool/net/EventLoop.h>
#include <proxy/ServerManager.h>
#include <string>
#include <utility>

using namespace cppnetool;
using namespace cppnetool::net;

using std::placeholders::_1;
using std::placeholders::_2;
using std::placeholders::_3;
class Session {
public:
	Session(ServerManager *serverManager, std::pair<TcpServer *, const std::string> *serverPair,
			TcpClient *tcpClient, std::string name);

	~Session();

	Timestamp& getClientTime()
	{ return clientTime_; }

	Timestamp& getServerTime()
	{ return serverTime_; }

	

private:
	void onTcpClientConnection_(TcpConnection *conn);
	void onTcpClientMessage_(TcpConnection *conn, Buffer *buf, Timestamp receiveTime);
	void onTcpServerMessage_(TcpConnection *conn, Buffer *buf, Timestamp receiveTime);
	void onTcpClientClose_(TcpConnection *conn);
	void onTcpServerClose_(TcpConnection *conn);

	TcpConnectionPtr getTcpClientConn_()
	{ return tcpClient_->getConn(); }

	TcpConnectionPtr getTcpServerConn_()
	{ return serverPair_->first->getConn(serverPair_->second); }

	std::string genKey_();

	ServerManager *serverManager_;
	typedef std::pair<TcpServer *, const std::string> ConnPair;
	ConnPair *serverPair_;
	TcpClient *tcpClient_;
	EventLoop *loop_;
	std::string name_;
	Timestamp clientTime_;
	Timestamp serverTime_;
	bool connecting_;
};

#endif /*SESSION_H*/