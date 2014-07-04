#include <iostream>
#include <proxy/Proxy.h>

#include <cppnetool/base/LogStream.h>

using namespace cppnetool;
int main()
{

	LogStream os;
	os << "hello";
	std::cout << "test" << std::endl;
	Proxy proxy(9888, "192.168.0.199", 80);
	proxy.set_conf();
	proxy.init();
	proxy.loop();
}