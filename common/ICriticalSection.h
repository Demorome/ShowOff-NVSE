#pragma once


#if 0
class ICriticalSection
{
	public:
		ICriticalSection()	{ InitializeCriticalSection(&critSection); }
		~ICriticalSection()	{ DeleteCriticalSection(&critSection); }

		void	Enter(void)		{ EnterCriticalSection(&critSection); }
		void	Leave(void)		{ LeaveCriticalSection(&critSection); }
		bool	TryEnter(void)	{ return TryEnterCriticalSection(&critSection) != 0; }

	private:
		CRITICAL_SECTION	critSection;
};

// Added thanks to JIP:
class ScopedLock  //Use this to be safe when doing non-thread-safe things.
{
	ICriticalSection* cs;

public:
	ScopedLock(ICriticalSection* _cs) : cs(_cs) { cs->Enter(); }
	~ScopedLock() { cs->Leave(); }
};
#endif

//Taken from xNVSE, probably mostly made by from JIP.
class ICriticalSection
{
	DWORD	owningThread;
	DWORD	enterCount;

public:
	ICriticalSection() : owningThread(0), enterCount(0) {}

	void Enter()
	{
		DWORD currThread = GetCurrentThreadId();
		if (owningThread == currThread)
		{
			enterCount++;
			return;
		}
		DWORD fastIdx = 10000;
		while (InterlockedCompareExchange(&owningThread, currThread, 0))
		{
			if (fastIdx)
			{
				fastIdx--;
				Sleep(0);
			}
			else Sleep(1);
		}
		enterCount = 1;
	}

	void Leave()
	{
		if (!--enterCount)
			owningThread = 0;
	}
};

class ScopedLock
{
	ICriticalSection* m_cs;

public:
	ScopedLock(ICriticalSection& cs) : m_cs(&cs) { cs.Enter(); }
	~ScopedLock() { m_cs->Leave(); }
};