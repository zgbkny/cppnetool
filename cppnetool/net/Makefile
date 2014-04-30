vpath %.cc examples
vpath %.h cppnetool/net
CPPFLAGS = -I ../..
OBJS = EventLoop.o 
EventLoop.o: EventLoop.cc
	g++ $(CPPFLAGS) -c $< -o $@ -std=c++11
clean:
	rm -f *.o
