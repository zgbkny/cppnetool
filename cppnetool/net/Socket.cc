#include <cppnetool/net/Socket.h>
#include <cppnetool/net/SocketOps.h>
#include <string.h>

using namespace cppnetool;
using namespace cppnetool::net;


Socket::~Socket()
{
	sockets::close(sockfd_);
}

void Socket::bindAddress(const InetAddress& localaddr)
{
	sockets::bindOrDie(sockfd_, localaddr.getSockAddrInet());
}

void Socket::listen()
{
	sockets::listenOrDie(sockfd_);
}

int Socket::accept(InetAddress* peeraddr)
{
	struct sockaddr_in addr;
	bzero(&addr, sizeof addr);
	int connfd = sockets::accept(sockfd_, &addr);
	if (connfd >= 0)
	{
		peeraddr->setSockAddrInet(addr);
	}
	return connfd;
}

void Socket::setReuseAddr(bool on)
{
	int optval = on ? 1 : 0;
	::setsockopt(sockfd_, SOL_SOCKET, SO_REUSEADDR,
					&optval, sizeof optval);
	// FIXME CHECK
}

void Socket::shutdownWrite()
{
	sockets::shutdownWrite(sockfd_);
}

