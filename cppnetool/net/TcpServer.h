#ifndef CPPNETOOL_NET_TCPSERVER_H
#define CPPNETOOL_NET_TCPSERVER_H

#include <cppnetool/net/Callbacks.h>
#include <cppnetool/net/TcpConnection.h>
#include <map>

namespace cppnetool
{
namespace net
{

class Acceptor;
class EventLoop;

class TcpServer
{
public:
	TcpServer(EventLoop *loop, const InetAddress &listenAddr);
	~TcpServer();

	void start();

	void setConnectionCallback(const ConnectionCallback &cb)
	{ connectionCallback_ = cb; }

	void setMessageCallback(const MessageCallback &cb)
	{ messageCallback_ = cb; }

private:
	void newConnection(int sockfd, const InetAddress &peerAddr);
	void removeConnection(TcpConnection *conn);

	typedef std::map<std::string, TcpConnectionPtr> ConnectionMap;

	EventLoop *loop_; // the acceptor loop
	const std::string name_;
	std::shared_ptr<Acceptor> acceptor_;
	ConnectionCallback connectionCallback_;
	MessageCallback messageCallback_;
	bool started_;
	int nextConnId_;
	ConnectionMap connections_;
};

}
}


#endif /*CPPNETOOL_NET_TCPSERVER_H*/