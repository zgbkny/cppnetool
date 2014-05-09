#ifndef CPPNETOOL_NET_ACCEPTOR_H
#define CPPNETOOL_NET_ACCEPTOR_H

#include <functional>
#include <cppnetool/net/InetAddress.h>
#include <cppnetool/net/Socket.h>
#include <cppnetool/net/Channel.h>

namespace cppnetool
{
namespace net
{
class EventLoop;

class Acceptor
{
public:
	typedef std::function<void (int sockfd, const InetAddress&)> NewConnectionCallback;

	Acceptor(EventLoop *loop, const InetAddress &listenAddr);
	void setNewConnectionCallback(const NewConnectionCallback &cb)
	{
		newConnectionCallback_ = cb;
	}

	bool listenning() const { return listenning_; }
	void listen();
private:
	void handleRead();

	EventLoop *loop_;
	Socket acceptSocket_;
	Channel acceptChannel_;
	NewConnectionCallback newConnectionCallback_;
	bool listenning_;

};

}
}

#endif /*CPPNETOOL_NET_ACCEPTOR_H*/