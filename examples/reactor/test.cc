#include <sys/timerfd.h>
#include <cppnetool/net/EventLoop.h>
#include <cppnetool/net/Channel.h>
#include <cppnetool/base/Logging.h>
#include <cppnetool/net/Acceptor.h>
#include <cppnetool/net/InetAddress.h>
#include <cppnetool/net/SocketOps.h>
#include <cppnetool/net/TcpServer.h>
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

void onConnection(TcpConnection *conn)
{
	if (conn->connected()) {
		cout << endl << "onConnection(): new connection [" << conn->name().c_str() << "] from " 
			 << conn->peerAddress().toHostPort().c_str() << endl;
	} else {
		cout << "onConnection(): connection [" << conn->name().c_str() << "] is down" << endl;
	}
}	

void onMessage(TcpConnection *conn, char *data, ssize_t len)
{
	cout << "onMessage(): received " << len << "bytes from connection [" << conn->name().c_str() << "]" << endl;
}

void server_test()
{
	InetAddress listenAddr(9981);
	EventLoop loop;
	
	TcpServer server(&loop, listenAddr);
	
	server.setConnectionCallback(onConnection);
	server.setMessageCallback(onMessage);
	server.start();

	loop.loop();
}

int main()
{
	//acceptor_test();
	server_test();
}

