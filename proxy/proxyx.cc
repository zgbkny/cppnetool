#include <iostream>
#include <proxy/Proxy.h>


int main()
{
	std::cout << "test" << std::endl;
	Proxy proxy(9888);
	proxy.set_conf();
	proxy.init();
	proxy.loop();
}