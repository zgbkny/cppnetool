#ifndef CPPNETOOL_NET_CALLBACKS_H
#define CPPNETOOL_NET_CALLBACKS_H
#include <cppnetool/base/Timestamp.h>
#include <functional>
#include <memory>
#include <sys/types.h>
namespace cppnetool
{

// All client visible callbacks go here.
namespace net
{

class Buffer;
class TcpConnection;
typedef std::shared_ptr<TcpConnection> TcpConnectionPtr;
typedef std::function<void()> TimerCallback;
typedef std::function<void(TcpConnection *)> ConnectionCallback; 
typedef std::function<void(TcpConnection *,
							Buffer *buf,
							Timestamp)> MessageCallback;
typedef std::function<void (TcpConnection *)> CloseCallback;

}


}

#endif  // CPPNETOOL_NET_CALLBACKS_H