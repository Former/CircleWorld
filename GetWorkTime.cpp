#include "GetWorkTime.h"

GetWorkTime::GetWorkTime(clockid_t a_ClockID)
{
	m_ClockID = a_ClockID;
	m_TimeStart = {0};
	m_TimeEnd = {0};
}
 
void GetWorkTime::Start()
{
	clock_gettime(m_ClockID, &m_TimeStart);
}

void GetWorkTime::End()
{
	clock_gettime(m_ClockID, &m_TimeEnd);
}

double GetWorkTime::GetWorkingTime()
{
	if (!m_TimeEnd.tv_sec)
		return GetCurrentTime();
	
	return GetDiffTime(m_TimeStart, m_TimeEnd);
}

double GetWorkTime::GetCurrentTime()
{
	timespec 	m_CurTime;
	clock_gettime(m_ClockID, &m_CurTime);
	return GetDiffTime(m_TimeStart, m_CurTime);
}

double GetWorkTime::GetDiffTime(const timespec& a_Start, const timespec& a_End)
{
	return ((a_End.tv_sec * 1.0e9 + a_End.tv_nsec) - (a_Start.tv_sec * 1.0e9 + a_Start.tv_nsec)) / 1.0e9;
}
