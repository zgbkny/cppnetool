#ifndef CPPNETOOL_BASE_LOGGING_H
#define CPPNETOOL_BASE_LOGGING_H

#include <cppnetool/base/debug.h>
#include <cppnetool/base/Timestamp.h>
#include <cppnetool/base/LogStream.h>

#include <vector>

namespace cppnetool
{

class Logger
{
public:
	enum LogLevel{
		TRACE,
		DEBUG,
		INFO,
		WARN,
		ERROR,
		FATAL,
		NUM_LOG_LEVELS,
	};

	class SourceFile
	{
	public:
		template<int N>
		inline SourceFile(const char (&arr)[N])
			:	data_(arr),
				size_(N-1)
		{
			const char *slash = strrchr(data_, '/');//builtin function
			if (slash) {
				data_ = slash + 1;
				size_ -= static_cast<int>(data_ - arr);
			}
		}

		explicit SourceFile(const char *filename)
			:	data_(filename)
		{
			const char *slash = strrchr(filename, '/');//builtin function
			if (slash) {
				data_ = slash + 1;
			}
			size_ -= static_cast<int>(strlen(data_));
		}

		const char *data_;
		int size_;
	};

	Logger(const char *file, int line);
	Logger(const char *file, int line, LogLevel level);
	Logger(const char *file, int line, LogLevel level, const char *func);
	Logger(const char *file, int line, bool toAbort);
	~Logger();

	LogStream& stream() { return impl_.stream_; }

	static LogLevel logLevel();
	static void setLogLevel(LogLevel level);

	typedef void (*OutputFunc)(const char *msg, int len);
	typedef void (*FlushFunc)();
	static void setOutput(OutputFunc);
	static void setFlush(FlushFunc);

private:

class Impl {
public:
	typedef Logger::LogLevel LogLevel;
	Impl(LogLevel level, int old_errno, const char *file, int line);
	void formatTime();
	void finish();

	Timestamp time_;
	LogStream stream_;
	LogLevel level_;
	int line_;
	const char *fullname_;
	const char *basename_;
};
	Impl impl_;
};
/*
#define LOG_TRACE std::cout
#define LOG_DEBUG std::cout
#define LOG_INFO std::cout
#define LOG_WARN std::cout
#define LOG_ERROR std::cout
#define LOG_FATAL std::cout
#define LOG_SYSERR std::cout
#define LOG_SYSFATAL std::cout*/

#define LOG_OUT std::cout

#define LOG_TRACE if (cppnetool::Logger::logLevel() <= cppnetool::Logger::TRACE) \
	cppnetool::Logger(__FILE__, __LINE__, cppnetool::Logger::TRACE, __func__).stream()
#define LOG_DEBUG if (cppnetool::Logger::logLevel() <= cppnetool::Logger::DEBUG) \
	cppnetool::Logger(__FILE__, __LINE__, cppnetool::Logger::DEBUG, __func__).stream()
#define LOG_INFO if (cppnetool::Logger::logLevel() <= cppnetool::Logger::INFO) \
	cppnetool::Logger(__FILE__, __LINE__).stream()
#define LOG_WARN cppnetool::Logger(__FILE__, __LINE__, cppnetool::Logger::WARN).stream()
#define LOG_ERROR cppnetool::Logger(__FILE__, __LINE__, cppnetool::Logger::ERROR).stream()
#define LOG_FATAL cppnetool::Logger(__FILE__, __LINE__, cppnetool::Logger::FATAL).stream()
#define LOG_SYSERR cppnetool::Logger(__FILE__, __LINE__, false).stream()
#define LOG_SYSFATAL cppnetool::Logger(__FILE__, __LINE__, true).stream()

const char* strerror_tl(int savedErrno);

}

#endif /*CPPNETOOL_BASE_LOGGING_H*/