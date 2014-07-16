#ifndef SESSION_H
#define SESSION_H

#include <cppnetool/net/TcpServer.h>
#include <string>
#include <utility>

class Session {
public:
	Session(std::pair<TcpServer *, const std::string> *serverPair);
private:
	std::pair<TcpServer *, const std::string> *serverPair_;
	TcpClient *tcpClient_;
	std::string name_;
};

#endif /*SESSION_H*/