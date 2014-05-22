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


int connect(int sockfd, const struct sockaddr_in &addr);
void bindOrDie(int sockfd, const struct sockaddr_in &addr);
void listenOrDie(int sockfd);
int accept(int sockfd, struct sockaddr_in *addr);
void close(int sockfd);
void shutdownWrite(int sockfd);

void fromHostPort(const char* ip, uint16_t port,
					struct sockaddr_in* addr);
void toHostPort(char* buf, size_t size,
					const struct sockaddr_in& addr);
struct sockaddr_in getLocalAddr(int sockfd);
struct sockaddr_in getPeerAddr(int sockfd);

int getSocketError(int sockfd);
bool isSelfConnect(int sockfd);
}
}
}


#endif /*CPPNETOOL_NET_SOCKETOPS_H*/
