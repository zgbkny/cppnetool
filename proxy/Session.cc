#include <proxy/Session.h>

Session::Session(std::pair<TcpServer *, const std::string> *serverPair, std::string name)
	:	serverPair_(serverPair),
		name_(name)
{

}