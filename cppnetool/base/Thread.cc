#include <iostream>
#include <cppnetool/base/Thread.h>
#include <unistd.h>
#include <sys/syscall.h>
#include <sys/types.h>
#include <linux/unistd.h>


namespace cppnetool
{

namespace CurrentThread
{
	__thread const char* t_threadName = "unknown";
}


namespace detail
{
	__thread pid_t t_cachedTid = 0;
	pid_t gettid()
	{
		return static_cast<pid_t>(::syscall(SYS_gettid));
	}

	struct ThreadData {
		typedef cppnetool::Thread::ThreadFunc ThreadFunc;
		ThreadFunc func_;
		std::string name_;
		std::weak_ptr<pid_t> wkTid_;

		ThreadData(const ThreadFunc &func, const std::string name, const std::shared_ptr<pid_t> tid)
			: func_(func), name_(name), wkTid_(tid)
		{ }

		void runInThread()
		{
			pid_t tid = cppnetool::CurrentThread::tid();
			std::shared_ptr<pid_t> ptid = wkTid_.lock();

			if (ptid) {
				*ptid = tid;
				ptid.reset();
			}

			func_();//FIXME:surround with try-catch
		} 

	};
	void *startThread(void *obj)
	{
		ThreadData *data = static_cast<ThreadData *>(obj);
		data->runInThread();
		delete data;
		return NULL;
	}
}


pid_t CurrentThread::tid()
{
	if (detail::t_cachedTid == 0)
	{
		detail::t_cachedTid = detail::gettid();
	}
	return detail::t_cachedTid;
}

AtomicInt32 Thread::numCreated_;

Thread::Thread(const ThreadFunc& func, const std::string& n)
	  : started_(false),
		joined_(false),
		pthreadId_(0),
		tid_(new pid_t(0)),
		func_(func),
		name_(n)
{
	numCreated_.increment();
}
Thread::~Thread()
{
	if (started_ && !joined_) {
		pthread_detach(pthreadId_);
	}
}

void Thread::start()
{
	assert(!started_);
	started_ = true;

	detail::ThreadData *data = new detail::ThreadData(func_, name_, tid_);
	if (pthread_create(&pthreadId_, NULL, detail::startThread, data)) {
		started_ = false;
		delete data;
		abort();
	}
}
void Thread::join()
{
	assert(started_);
	assert(!joined_);
	joined_ = true;
	pthread_join(pthreadId_, NULL);
}

}
