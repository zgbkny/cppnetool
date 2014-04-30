vpath %.cc examples
vpath %.h cppnetool/base
CPPFLAGS = -I ../..
OBJS = Thread.o 
Thread.o: Thread.cc
	g++ $(CPPFLAGS) -c $< -o $@ -std=c++11
clean:
	rm -f *.o
