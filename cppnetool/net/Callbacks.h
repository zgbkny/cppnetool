#ifndef CPPNETOOL_NET_CALLBACKS_H
#define CPPNETOOL_NET_CALLBACKS_H

namespace cppnetool
{

// All client visible callbacks go here.

typedef std::function<void()> TimerCallback;

}

#endif  // CPPNETOOL_NET_CALLBACKS_H