#pragma once
#include <time.h>

class GetWorkTime
{
public:
    GetWorkTime(clockid_t clockID = CLOCK_THREAD_CPUTIME_ID);

    void Start();
    void End();

    double GetWorkingTime();
	double GetCurrentTime();
	
private:
	static double GetDiffTime(const timespec& a_Start, const timespec& a_End);

	clockid_t 	m_ClockID;
	timespec 	m_TimeStart;
	timespec 	m_TimeEnd;
	
	bool m_IsStarted;
	bool m_IsEnded;
};
