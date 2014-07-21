#ifndef PROXY_SERVERMANAGER_H
#define PROXY_SERVERMANAGER_H
#include <cppnetool/net/TcpClient.h>
#include <cppnetool/net/InetAddress.h>
#include <cppnetool/net/EventLoop.h>
#include <cppnetool/net/TcpConnection.h>
#include <cppnetool/net/Callbacks.h>
#include <string>
#include <queue>

using namespace cppnetool;
using namespace cppnetool::net;
using std::placeholders::_1;
class ServerManager {
public:
	ServerManager(std::string ip, int port, EventLoop *loop);
	TcpClient *getTcpClient();

	bool init(int size);

	BufferPtr getRequest(std::string &key) {
		return requestMap_[key];
	}
	bool addRequest(std::string key, BufferPtr bufferPtr) {
		if (requestMap_[key] == NULL) {
			requestMap_[key] = bufferPtr;
			return true;
		} else {
			return false;
		}
	}
	BufferPtr getResponse(std::string &key) {
		return responseMap_[key];
	}

private:
	void onTcpClientConnection_(TcpConnection *conn);
	typedef std::map<std::string, BufferPtr> BufferMap; 

	InetAddress serverAddr_;
	EventLoop *loop_;

	std::queue<TcpClient *> newTcpClients_;
	std::queue<TcpClient *> activeTcpClients_;
	BufferMap requestMap_;
	BufferMap responseMap_;
};

#endif /*PROXY_SERVERMANAGER_H*/