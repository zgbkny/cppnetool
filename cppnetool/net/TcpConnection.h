#ifndef CPPNETOOL_NET_TCPCONNECTION_H
#define CPPNETOOL_NET_TCPCONNECTION_H

#include <cppnetool/net/Callbacks.h>
#include <cppnetool/net/InetAddress.h>
#include <cppnetool/net/Buffer.h>

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


	//void send(const void* message, size_t len);
	// Thread safe.
	void send(const std::string& message);
	// Thread safe.
	void shutdown();

	void setTcpNoDelay(bool on);
	void setKeepAlive(bool on);

	void setConnectionCallback(const ConnectionCallback& cb)
	{ connectionCallback_ = cb; }

	void setMessageCallback(const MessageCallback& cb)
	{ messageCallback_ = cb; }

	void setWriteCompleteCallback(const WriteCompleteCallback cb)
	{ writeCompleteCallback_ = cb; }

	void setHighWaterMarkCallback(const HighWaterMarkCallback cb)
	{ highWaterMarkCallback_ = cb; }

	/// Internal use only.
	void setCloseCallback(const CloseCallback& cb)
	{ closeCallback_ = cb; }

	// called when TcpServer accepts a new connection
	void connectEstablished();   // should be called only once
	void connectDestroyed();  // should be called only once

private:
	enum StateE { kConnecting, kConnected, kDisconnecting, kDisconnected };

	void setState(StateE s) { state_  = s; }
	void handleRead(Timestamp receiveTime);
	void handleWrite();
	void handleClose();
	void handleError();
	void sendInLoop(const std::string& message);
	void shutdownInLoop();

	EventLoop *loop_;
	std::string name_;
	StateE state_;
	std::shared_ptr<Socket> socket_;
	std::shared_ptr<Channel> channel_;
	InetAddress localAddr_;
	InetAddress peerAddr_;
	ConnectionCallback connectionCallback_;
	MessageCallback messageCallback_;
	WriteCompleteCallback writeCompleteCallback_;
	HighWaterMarkCallback highWaterMarkCallback_;
	CloseCallback closeCallback_;
	Buffer inputBuffer_;
	Buffer outputBuffer_;
};
}
}


#endif /*cppnetool_NET_TCPCONNECTION_H*/