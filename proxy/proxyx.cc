#include <iostream>
#include <proxy/Proxy.h>

using namespace cppnetool;

EventLoop *loop;
TcpClient *tcpClient;
TcpServer *tcpServer;
int i;

void onTcpClientConnection(TcpConnection *conn)
{
	if (conn->connected()) {
		LOG_DEBUG << "onTcpClientConnection(): new connection [" << conn->name().c_str() << "] from " 
			 << conn->peerAddress().toHostPort().c_str();
	} else {
		LOG_DEBUG << "onTcpClientConnection(): connection [" << conn->name().c_str() << "] is down";
	}
}

void onTcpClientMessage(TcpConnection *conn, Buffer *buf, Timestamp receiveTime)
{
	LOG_DEBUG << "onTcpClientMessage(): received" << buf->readableBytes() 
			  << "bytes from connection [" << conn->name()
			  << "] at " << receiveTime.toFormattedString();
	
	conn->shutdown();
}

void timeout() {
	i++;
	LOG_DEBUG << "here" ;
	TcpConnectionPtr connPtr = tcpClient->getConn();
	if (connPtr != NULL) {
		LOG_DEBUG << "connPtr not NULL! " << i << " ";
		if (i > 10) {
			string data("get");
			connPtr->send(data);
		}
	} else {
		LOG_DEBUG << "connPtr NULL!";
		//tcpClient->connect();
	}
}


void testTcpClient() {

	loop = new EventLoop();
	InetAddress serverAddr("192.168.0.137", 8080);
	tcpClient = new TcpClient(loop, serverAddr);
	tcpClient->setConnectionCallback(onTcpClientConnection);
	tcpClient->setMessageCallback(onTcpClientMessage);
	tcpClient->connect();
	loop->runEvery(3, timeout);
	loop->loop();
}

void testTcpServer() {
	loop = new EventLoop();
	InetAddress listenAddr(8080);
	tcpServer = new TcpServer(loop, listenAddr);
	tcpServer->setConnectionCallback(onTcpClientConnection);
	tcpServer->setMessageCallback(onTcpClientMessage);
	tcpServer->start();
	loop->loop();
}

void proxy() {
	Proxy proxy(80, "192.168.0.199", 80);
	proxy.set_conf();
	proxy.init();
	proxy.loop();
}

int main()
{
	proxy();
	//testTcpClient();
	//testTcpServer();
}