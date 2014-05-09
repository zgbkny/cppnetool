#ifndef CPPNETOOL_NET_SOCKETOPS_H
#define CPPNETOOL_NET_SOCKETOPS_H

#include <arpa/inet.h>
#include <endian.h>
namespace cppnetool
{
namespace net
{
namespace sockets
{

int createNonblockingOrDie();

void bindOrDie(int sockfd, const struct sockaddr_in &addr);
void listenOrDie(int sockfd);
int accept(int sockfd, struct sockaddr_in *addr);
void close(int sockfd);

void fromHostPort(const char* ip, uint16_t port,
					struct sockaddr_in* addr);
void toHostPort(char* buf, size_t size,
					const struct sockaddr_in& addr);

}
}
}


#endif /*CPPNETOOL_NET_SOCKETOPS_H*/
