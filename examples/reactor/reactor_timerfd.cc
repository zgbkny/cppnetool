#include <sys/timerfd.h>
#include <cppnetool/net/EventLoop.h>
#include <cppnetool/net/Channel.h>
#include <cppnetool/base/Logging.h>
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

int main()
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

