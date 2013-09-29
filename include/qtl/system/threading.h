#if !defined (_THREADING_H)
#define _THREADING_H

#include "system.h"

#if _QTL_USE_STD_THREADING
#	include <mutex>
#   include <thread>
#endif

#if _QTL_OS_UNIX
#   include <sys/stat.h>
#   include <sys/time.h>
#elif _QTL_OS_WINDOWS
#   include <direct.h>
#   include <Windows.h>
#endif

namespace Qtl { namespace System { namespace Threading {

#if !_QTL_USE_STD_THREADING

class Mutex
{
private:

#if _QTL_OS_UNIX
	pthread_mutex_t 	_underlyingMutex;
#elif _QTL_OS_WINDOWS
	CRITICAL_SECTION 	_underlyingMutex;
#endif
	
public:
	Mutex()
	{
#if _QTL_OS_UNIX
        pthread_mutex_init(&_underlyingMutex, NULL);
#elif _QTL_OS_WINDOWS
        InitializeCriticalSection(&_underlyingMutex);
#endif
	}
	
	virtual ~Mutex()
	{
#if _QTL_OS_UNIX
        pthread_mutex_destroy(&_underlyingMutex);
#elif _QTL_OS_WINDOWS
        DeleteCriticalSection(&_underlyingMutex);
#endif
	}
	
	void Lock()
	{
#if _QTL_OS_UNIX
        pthread_mutex_lock(&_underlyingMutex);
#elif _QTL_OS_WINDOWS
        EnterCriticalSection(&_underlyingMutex);
#endif
	}
	
	bool TryLock()
	{
#if _QTL_OS_UNIX
		return (pthread_mutex_trylock(&_underlyingMutex) == 0) ? true : false;
#elif _QTL_OS_WINDOWS
		return TryEnterCriticalSection(&_underlyingMutex) ? true : false;
#endif	
	}
	
	void Unlock()
	{
#if _QTL_OS_UNIX
		pthread_mutex_unlock(&_underlyingMutex);
#elif _QTL_OS_WINDOWS
		LeaveCriticalSection(&_underlyingMutex);
#endif
	}
};

template <class TMutex=Mutex>
class ScopedLock
{
public:
	typedef TMutex InnerMutex;

private:
	InnerMutex *_mutex;
	
private:
	ScopedLock() {}
	
public:
	explicit ScopedLock(InnerMutex &mutex)
	{
		_mutex = &mutex;
		_mutex->Lock();
	}
	
	~ScopedLock()
	{
		_mutex->Unlock();
	}
};

typedef ScopedLock<Mutex> LockGuard;

#else

typedef std::mutex	Mutex;
typedef std::lock_guard<Mutex>	LockGuard;

#endif

}}}

#endif
