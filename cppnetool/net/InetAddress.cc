#include <cppnetool/net/InetAddress.h>
#include <cppnetool/net/SocketOps.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>

using namespace cppnetool;
using namespace cppnetool::net;

static const in_addr_t kInaddrAny = INADDR_ANY;

//BOOST_STATIC_ASSERT(sizeof(InetAddress) == sizeof(struct sockaddr_in));

InetAddress::InetAddress(uint16_t port)
{
	bzero(&addr_, sizeof addr_);
	addr_.sin_family = AF_INET;
	addr_.sin_addr.s_addr = htonl(kInaddrAny);
	addr_.sin_port = htons(port);
}



InetAddress::InetAddress(const std::string& ip, uint16_t port)
{
	bzero(&addr_, sizeof addr_);
	sockets::fromHostPort(ip.c_str(), port, &addr_);
}

std::string InetAddress::toHostPort()
{
	char buf[32];
	sockets::toHostPort(buf, sizeof buf, addr_);
	return buf;
}
