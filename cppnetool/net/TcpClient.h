#ifndef CPPNETOOL_NET_TCPCLIENT_H
#define CPPNETOOL_NET_TCPCLIENT_H

#include <cppnetool/base/Mutex.h>
#include <cppnetool/net/InetAddress.h>
#include <cppnetool/net/Callbacks.h>

namespace cppnetool
{
namespace net
{
class Connector;
class EventLoop;

typedef std::shared_ptr<Connector> ConnectorPtr;

class TcpClient
{
public:
	TcpClient(EventLoop *loop,
				const InetAddress &serverAddr);
	~TcpClient();

	void connect();
	void disconnect();
	void stop();

	TcpConnectionPtr connection() const
	{
		MutexLockGuard lock(mutex_);
		return connection_;
	}

	bool retry() const;
	void enableRetry() { retry_ = true; }

	/// Set connection callback.
	/// Not thread safe.
	void setConnectionCallback(const ConnectionCallback& cb)
	{ connectionCallback_ = cb; }

	/// Set message callback.
	/// Not thread safe.
	void setMessageCallback(const MessageCallback& cb)
	{ messageCallback_ = cb; }

	/// Set write complete callback.
	/// Not thread safe.
	void setWriteCompleteCallback(const WriteCompleteCallback& cb)
	{ writeCompleteCallback_ = cb; }

	TcpConnectionPtr getConn() {
		if (state_) return connection_;
		else return NULL;
	}

private:
	void newConnection(int sockfd);
	void removeConnection(TcpConnection *conn);

	EventLoop *loop_;
	ConnectorPtr connector_; // avoid revealing Connector
	ConnectionCallback connectionCallback_;
	MessageCallback messageCallback_;
	WriteCompleteCallback writeCompleteCallback_;
	bool retry_;   // atmoic
	bool connect_; // atomic
	bool state_;
	// always in loop thread
	int nextConnId_;
	mutable MutexLock mutex_;
	TcpConnectionPtr connection_; // @BuardedBy mutex_
};


}
}

#endif /*CPPNETOOL_NET_TCPCLIENT_H*/