#include <cppnetool/base/Logging.h>
#include <cppnetool/base/Timestamp.h>
#include <cppnetool/base/Thread.h>
#include <cppnetool/base/debug.h>

#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <fcntl.h>

#include <sstream>

namespace cppnetool
{
	__thread char t_errnobuf[512];
	__thread char t_time[32];
	__thread time_t t_lastSecond;

	const int PIDS = 20;

	FILE *file_fd[PIDS];

	const char *strerror_tl(int savedErrno)
	{
		return strerror_r(savedErrno, t_errnobuf, sizeof t_errnobuf);
	}

	Logger::LogLevel initLogLevel()
	{
		return Logger::TRACE;

		if (::getenv("CPPNETOOL_LOG_TRACE"))
			return Logger::TRACE;
		else 
			return Logger::DEBUG;
	}

	Logger::LogLevel g_logLevel = initLogLevel();

	const char *LogLevelName[Logger::NUM_LOG_LEVELS] =
	{
		"TRACE ",
		"DEBUG ",
		"INFO  ",
		"WARN  ",
		"ERROR ",
		"FATAL ",
	};


	void defaultOutput(const char* msg, int len)
	{
		size_t n = fwrite(msg, 1, len, stdout);
		(void)n;
	}

	void defaultFlush()
	{
		fflush(stdout);
	}

	void pidOutput(const char* msg, int len)
	{
		int pid = CurrentThread::tid();
		if (file_fd[pid % PIDS] == NULL) {
			std::string file = "thread_";
			char pids[6] = {0, 0, 0, 0, 0, 0};
			sprintf(pids, "%d", pid);
			file += pids;
			file += "_log.txt";
			FILE *ret = fopen(file.c_str(), "w");
			if (ret != NULL) {
				file_fd[pid % PIDS] = ret;
			} else {
				file_fd[pid % PIDS] = stdout;
			}
		}
		size_t n = fwrite(msg, 1, len, file_fd[pid % PIDS]);
		(void)n;
	}

	void pidFlush()
	{
		int pid = CurrentThread::tid();
		fflush(file_fd[pid % PIDS]);
	}

	Logger::OutputFunc g_output = pidOutput;
	Logger::FlushFunc g_flush = pidFlush;
}


using namespace cppnetool;
Logger::Impl::Impl(LogLevel level, int savedErrno, const char *file, int line)
	:	time_(Timestamp::now()),
		stream_(),
		level_(level),
		line_(line),
		fullname_(file),
		basename_(NULL)
{
	//find the last '/' pos in fullname
	const char *path_sep_pos = strrchr(fullname_, '/');
	basename_ = (path_sep_pos != NULL) ? path_sep_pos + 1 : fullname_;

	formatTime();
	Fmt tid("tid:%5d ", CurrentThread::tid());
	assert(tid.length() == 10);
	stream_ << T(tid.data(), 10);
	stream_ << T(LogLevelName[level], 6);
	if (savedErrno != 0)
	{
		stream_ << strerror_tl(savedErrno) << " (errno= " <<  savedErrno << ") ";
	}
}

void Logger::Impl::formatTime()
{
	int64_t microSecondSinceEpoch = time_.microSecondsSinceEpoch();
	time_t seconds = static_cast<time_t>(microSecondSinceEpoch / 1000000);
	int microseconds = static_cast<int>(microSecondSinceEpoch % 1000000);
	if (seconds != t_lastSecond)
	{
		t_lastSecond = seconds;
		struct tm tm_time;
		::gmtime_r(&seconds, &tm_time); // 

		int len = snprintf(t_time, sizeof(t_time), "%4d%02d%02d %02d:%02d:%02d",
        	tm_time.tm_year + 1900, tm_time.tm_mon + 1, tm_time.tm_mday,
        	tm_time.tm_hour, tm_time.tm_min, tm_time.tm_sec);
    	assert(len == 17); (void)len;
	}
	Fmt us(".%06dZ ", microseconds);
	assert(us.length() == 9);
	stream_ << T(t_time, 17) << T(us.data(), 9);
}

void Logger::Impl::finish()
{
 	stream_ << " - " << basename_ << ':' << line_ << '\n';
}

Logger::Logger(const char* file, int line)
	:	impl_(INFO, 0, file, line)
{
}

Logger::Logger(const char* file, int line, LogLevel level, const char* func)
 	:	impl_(level, 0, file, line)
{
	impl_.stream_ << func << ' ';
}

Logger::Logger(const char* file, int line, LogLevel level)
	:	impl_(level, 0, file, line)
{
}

Logger::Logger(const char* file, int line, bool toAbort)
	:	impl_(toAbort ? FATAL : ERROR, errno, file, line)
{
}

Logger::~Logger()
{
	impl_.finish();
	const LogStream::Buffer& buf(stream().buffer());
	g_output(buf.data(), buf.length());
	g_flush();
	if (impl_.level_ == FATAL)
	{
		g_flush();
		abort();
	}
}

Logger::LogLevel Logger::logLevel()
{
	return g_logLevel;
}

void Logger::setLogLevel(Logger::LogLevel level)
{
	g_logLevel = level;
}

void Logger::setOutput(OutputFunc out)
{
	g_output = out;
}

void Logger::setFlush(FlushFunc flush)
{
	g_flush = flush;
}

