#ifndef __TIMER_H__
#define __TIMER_H__

#include <ctime>

class Timer
{
public:
	inline Timer(): mTime(0)
	{
	
	}
	
	inline void Start()
	{
		mTime = clock();
	}
	
	inline void Stop()
	{
		mTime = clock() - mTime;
	}
	
	template<typename T>
	inline T GetTimeSpendSecond()const
	{
		return (T)mTime / (T)CLOCKS_PER_SEC;
	}
	
	inline clock_t GetTimeSpendMillisecond()const
	{
		return mTime;
	}
	
private:
	clock_t mTime;
};

#endif // __TIMER_H__