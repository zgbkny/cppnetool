#ifndef CPPNETOOL_BASE_MUTEX_H
#define CPPNETOOL_BASE_MUTEX_H


#include <cppnetool/base/Thread.h>
#include <cppnetool/base/debug.h>
#include <cppnetool/base/Logging.h>
#include <pthread.h>


namespace cppnetool
{
class MutexLock
{
public:
	MutexLock() : holder_(0) {
		pthread_mutex_init(&mutex_, NULL);
	}

	~MutexLock()
	{
		LOG_TRACE << "~MutexLock";
		assert(holder_ == 0);
		pthread_mutex_destroy(&mutex_);
	}

	bool isLockedByThisThread()
	{
		return holder_ == CurrentThread::tid();
	}

	void assertLocked()
	{
		assert(isLockedByThisThread());
	}

	void lock()
	{
		pthread_mutex_lock(&mutex_);
		holder_ = CurrentThread::tid();
	}

	void unlock()
	{
		holder_ = 0;
		pthread_mutex_unlock(&mutex_);
	}

	pthread_mutex_t *getPthreadMutex()
	{
		return &mutex_;
	}

private:
	pthread_mutex_t mutex_;
	pid_t holder_;
};

class MutexLockGuard
{
public:
	explicit MutexLockGuard(MutexLock &mutex)
		:	mutex_(mutex)
	{
		mutex_.lock();
	}

	~MutexLockGuard()
	{
		mutex_.unlock();
	}
private:
	MutexLock &mutex_;
};

}

#endif /*CPPNETOOL_BASE_MUTEX_H*/