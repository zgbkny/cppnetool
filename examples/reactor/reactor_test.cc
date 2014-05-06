#include <cppnetool/net/EventLoop.h>
#include <cppnetool/base/Thread.h>
#include <stdio.h>

#include <unistd.h>
#include <sys/syscall.h>
#include <sys/types.h>
#include <linux/unistd.h>

/*
cppnetool::net::EventLoop* g_loop;
void threadFunc()
{
  printf("threadFunc(): pid = %d, tid = %d\n",
         static_cast<pid_t>(::syscall(SYS_gettid)), cppnetool::CurrentThread::tid());

  cppnetool::net::EventLoop loop;
  loop.loop();
}
void test1() {
  printf("main(): pid = %d, tid = %d\n",
         static_cast<pid_t>(::syscall(SYS_gettid)), cppnetool::CurrentThread::tid());

  cppnetool::net::EventLoop loop;

  cppnetool::Thread thread(threadFunc);
  thread.start();

  loop.loop();
  pthread_exit(NULL);
}
void threadFunc2()
{
  g_loop->loop();
}

void test2()
{
  cppnetool::net::EventLoop loop;
  g_loop = &loop;
  cppnetool::Thread t(threadFunc2);
  t.start();
  t.join();
}
int main1()
{
  test2();
}*/
