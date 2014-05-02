#ifndef CPPNETOOL_NET_TCPCONNECTION_H
#define CPPNETOOL_NET_TCPCONNECTION_H

namespace cppnetool
{
namespace net
{
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

	const InetAddress& localAddress() { return localAddr_; }
	const InetAddress& peerAddress() { return peerAddr_; }
private:
	InetAddress localAddr_;
	InetAddress peerAddr_;
};
}
}


#ifndef /*cppnetool_NET_TCPCONNECTION_H*/