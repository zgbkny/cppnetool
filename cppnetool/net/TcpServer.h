#ifndef CPPNETOOL_NET_TCPSERVER_H
#define CPPNETOOL_NET_TCPSERVER_H

#include <cppnetool/net/Callbacks.h>
#include <cppnetool/net/TcpConnection.h>

namespace cppnetool
{
namespace net
{

class TcpServer
{
public:
	TcpServer(EventLoop *loop, const InetAddress &listenAddr);
	~TcpServer();

	void start();

	void setConnectionCallback(const ConnctionCallback &cb);
	{ connectionCallback_ = cb; }

	void setMessageCallback(const MessageCallback &cb);
	{ messageCallback_ = cb; }

private:
	vodi newConnection(int sockfd, const InetAddress &peerAddr);

	typedef std::map<std::string, TcpConnectionPtr> ConnectionMap;

	EventLoop *loop_; // the acceptor loop
	const std::string name_;
	std::shared_ptr<Acceptor> acceptor_;
	ConnectionCallback connectionCallback_;
	MessageCallback messageCallback_;
	bool started_;
	int nextConnId_;
	ConnectionMap connctions_;
};

}
}


#endif /*CPPNETOOL_NET_TCPSERVER_H*/