#pragma once

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