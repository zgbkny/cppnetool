#ifndef CPPNETOOL_NET_TCPCONNECTION_H
#define CPPNETOOL_NET_TCPCONNECTION_H

#include <cppnetool/net/Callbacks.h>
#include <cppnetool/net/InetAddress.h>

namespace cppnetool
{
namespace net
{

class Channel;
class EventLoop;
class Socket;

class TcpConnection
{
public:

	TcpConnection(EventLoop* loop,
				const std::string& name,
				int sockfd,
				const InetAddress& localAddr,
				const InetAddress& peerAddr);
	~TcpConnection();

	EventLoop* getLoop() const { return loop_; }
	const std::string& name() const { return name_; }
	const InetAddress& localAddress() { return localAddr_; }
	const InetAddress& peerAddress() { return peerAddr_; }
	bool connected() const { return state_ == kConnected; }

	void setConnectionCallback(const ConnectionCallback& cb)
		{ connectionCallback_ = cb; }

	void setMessageCallback(const MessageCallback& cb)
		{ messageCallback_ = cb; }
	// called when TcpServer accepts a new connection
	void connectEstablished();   // should be called only once

private:
	enum StateE { kConnecting, kConnected };

	void setState(StateE s) { state_  = s; }
	void handleRead();

	EventLoop *loop_;
	std::string name_;
	StateE state_;
	std::shared_ptr<Socket> socket_;
	std::shared_ptr<Channel> channel_;
	InetAddress localAddr_;
	InetAddress peerAddr_;
	ConnectionCallback connectionCallback_;
	MessageCallback messageCallback_;
};
}
}


#endif /*cppnetool_NET_TCPCONNECTION_H*/