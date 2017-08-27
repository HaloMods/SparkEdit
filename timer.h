#ifndef _TIMER_H_
#define _TIMER_H_

#include "windows.h"

#define NAME_FORMAT_DAYFIRST  1     //DDMMMYYYY_HHMMSS
#define NAME_FORMAT_YEARFIRST 2     //YYYYMMMDD_HHMMSS
#define NAME_FORMAT_LONGDAY   3     //YYYY_DDD_HHMMSS

class CTimer
{
public:
  CTimer();
  ~CTimer();

  double GetLowResElapsed(void);
  void ResetLowResTimer(void);
  double GetElapsed(void);
  void ResetTimer(void);
  double GetDeltaTime(void);
  void Delay(double);

  CString GetCurrentSystemTime(void);
  void GetCurrentSystemTime(UINT &yr, UINT &month, UINT &day, UINT &hour,
                            UINT &min, float &sec);
  void GetCurrentSystemTime(struct tm &time_struct);

  CString GetBaseFileName(int nNameOptn);
  CString ConvertMonthToString(UINT nMonth);

protected:
  __int64 m_TicksPerSecond;
  __int64 m_BeginTime64;
  __int64 m_EndTime64;
  BOOL m_bTimerStarted;
  __int64 m_Timeout64;
  
  time_t m_LowResBeginTime;
  time_t m_LowResEndTime;
};

#endif
