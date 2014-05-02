#include <cppnetool/net/InetAddress.h>
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

void InetAddress::fromHostPort(const char* ip, uint16_t port,
                           struct sockaddr_in* addr)
{
	addr->sin_family = AF_INET;
	addr->sin_port = htons(port);
	if (::inet_pton(AF_INET, ip, &addr->sin_addr) <= 0)
	{
	//LOG_SYSERR << "sockets::fromHostPort";
	}
}

void InetAddress::toHostPort(char* buf, size_t size,
                         const struct sockaddr_in& addr)
{
	char host[INET_ADDRSTRLEN] = "INVALID";
	::inet_ntop(AF_INET, &addr.sin_addr, host, sizeof host);
	uint16_t port = ntohs(addr.sin_port);
	snprintf(buf, size, "%s:%u", host, port);
}

InetAddress::InetAddress(const std::string& ip, uint16_t port)
{
	bzero(&addr_, sizeof addr_);
	fromHostPort(ip.c_str(), port, &addr_);
}

std::string InetAddress::toHostPort()
{
	char buf[32];
	toHostPort(buf, sizeof buf, addr_);
	return buf;
}
