#include <cppnetool/net/Connector.h>
#include <cppnetool/net/EventLoop.h>
#include <cppnetool/net/Channel.h>
#include <errno.h>

using namespace cppnetool;
using namespace cppnetool::net;

const int Connector::kMaxRetryDelayMs;

Connector::Connector(EventLoop *loop, const InetAddress &serverAddr)
	:	loop_(loop),
		serverAddr_(serverAddr),
		connect_(false),
		state_(kDisconnected),
		retryDelayMs_(kInitRetryDelayMs)
{
	LOG_DEBUG << "ctor[" << this << "]";
}
Connector::~Connector()
{
	
}