#ifndef SESSION_H
#define SESSION_H

#include <cppnetool/net/TcpServer.h>
#include <cppnetool/net/TcpClient.h>
#include <cppnetool/net/TcpConnection.h>
#include <string>
#include <utility>
using namespace cppnetool;
using namespace cppnetool::net;

using std::placeholders::_1;
using std::placeholders::_2;
using std::placeholders::_3;
class Session {
public:
	Session(std::pair<TcpServer *, const std::string> *serverPair,
			TcpClient *tcpClient, std::string name);


private:
	//void onTcpClientConnection_(TcpConnection *conn);
	//void onTcpClientMessage_(TcpConnection *conn, Buffer *buf, Timestamp receiveTime);
	void onTcpServerMessage_(TcpConnection *conn, Buffer *buf, Timestamp receiveTime);

	std::pair<TcpServer *, const std::string> *serverPair_;
	TcpClient *tcpClient_;
	std::string name_;
};

#endif /*SESSION_H*/