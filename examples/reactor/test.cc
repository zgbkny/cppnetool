#include <sys/timerfd.h>
#include <cppnetool/net/EventLoop.h>
#include <cppnetool/net/Channel.h>
#include <cppnetool/base/Logging.h>
#include <cppnetool/net/Acceptor.h>
#include <cppnetool/net/InetAddress.h>
#include <cppnetool/net/SocketOps.h>
#include <iostream>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
using namespace cppnetool;
using namespace cppnetool::net;
using namespace std;

EventLoop * g_loop;

void timeout()
{
	cout << "timeout" << endl;
	g_loop->quit();
}

void timefd_test()
{
	EventLoop loop;
	g_loop = &loop;

	int timerfd = timerfd_create(CLOCK_MONOTONIC, TFD_NONBLOCK | TFD_CLOEXEC);
	Channel channel(&loop, timerfd);

	channel.setReadCallback(timeout);
	cout << "here" << endl;
	channel.enableReading();
	cout << "here" << endl;
	struct itimerspec howlong;
	bzero(&howlong, sizeof howlong);
	howlong.it_value.tv_sec = 5;
	timerfd_settime(timerfd, 0, &howlong, NULL);
	cout << "here" << endl;
	loop.loop();
	LOG_TRACE << "here" << endl;
	close(timerfd);
}

void newConnection(int sockfd, const InetAddress &peerAddr)
{
	cout << "new connection" << peerAddr.toHostPort().c_str() << endl;
	write(sockfd, "how are you?\n", 13);
	sockets::close(sockfd);
}

void acceptor_test()
{
	InetAddress listenAddr(9981);
	EventLoop loop;
	Acceptor acceptor(&loop, listenAddr);
	
	acceptor.setNewConnectionCallback(newConnection);
	acceptor.listen();

	loop.loop();
}

int main()
{
	acceptor_test();
}

