#ifndef CPPNETOOL_BASE_MUTEX_H
#define CPPNETOOL_BASE_MUTEX_H

#include <assert.h>
#include <pthread.h>

namespace cppnetool
{
	class MutexLock
	{
	public:
		MutexLock() : holder_(0) {

		}
	private:
		pthread_mutex_t mutex_;
  		pid_t holder_;
	};
}

#endif /*CPPNETOOL_BASE_MUTEX_H*/