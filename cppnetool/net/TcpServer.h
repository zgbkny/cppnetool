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
class EventLoopThreadPool;

class TcpServer
{
public:


	TcpServer(EventLoop *loop, const InetAddress &listenAddr);
	~TcpServer();

	/// Set the number of threads for handling input.
	///
	/// Always accepts new connection in loop's thread.
	/// Must be called before @c start
	/// @param numThreads
	/// - 0 means all I/O in loop's thread, no thread will created.
	///   this is the default value.
	/// - 1 means all I/O in another thread.
	/// - N means a thread pool with N threads, new connections
	///   are assigned on a round-robin basis.
	void setThreadNum(int numThreads);

	void start();

	void setConnectionCallback(const ConnectionCallback &cb)
	{ connectionCallback_ = cb; }

	void setMessageCallback(const MessageCallback &cb)
	{ messageCallback_ = cb; }

	void setWriteCompleteCallback(const WriteCompleteCallback& cb)
	{ writeCompleteCallback_ = cb; }

	TcpConnectionPtr getConn(std::string name) { return connections_[name]; }

private:

	/// Not thread safe, but in loop
	void newConnection(int sockfd, const InetAddress &peerAddr);
	/// Thread safe
	void removeConnection(TcpConnection *conn);
	/// Not thread safe, but in loop
	void removeConnectionInLoop(TcpConnection *conn);

	typedef std::map<std::string, TcpConnectionPtr> ConnectionMap;
	EventLoop *loop_; // the acceptor loop
	const std::string name_;
	std::shared_ptr<Acceptor> acceptor_;
	std::shared_ptr<EventLoopThreadPool> threadPool_;
	ConnectionCallback connectionCallback_;
	MessageCallback messageCallback_;
	WriteCompleteCallback writeCompleteCallback_;
	bool started_;
	int nextConnId_;
	ConnectionMap connections_;
};

}
}


#endif /*CPPNETOOL_NET_TCPSERVER_H*/