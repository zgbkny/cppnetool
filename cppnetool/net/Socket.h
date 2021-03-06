#ifndef CPPNETOOL_NET_SOCKET_H
#define CPPNETOOL_NET_SOCKET_H
#include <cppnetool/net/InetAddress.h>
namespace cppnetool
{
namespace net
{

class Socket
{
public:
	explicit Socket(int sockfd)
	: sockfd_(sockfd)
	{ }

	Socket() { }

	~Socket();

	int fd() const { return sockfd_; }

	/// abort if address in use
	void bindAddress(const InetAddress& localaddr);
	/// abort if address in use
	void listen();

	/// On success, returns a non-negative integer that is
	/// a descriptor for the accepted socket, which has been
	/// set to non-blocking and close-on-exec. *peeraddr is assigned.
	/// On error, -1 is returned, and *peeraddr is untouched.
	int accept(InetAddress* peeraddr);


	///
	/// Enable/disable SO_REUSEADDR
	///
	void setReuseAddr(bool on);

	void shutdownWrite();

	void shutdownRead();

	void setTcpNoDelay(bool on);

	void setKeepAlive(bool on);

private:
	int sockfd_;
};

}
}

#endif /*cppnetool_NET_SOCKET_H*/
