#include "stdafx.h"
#include "timer.h"
#include <time.h>
#include <sys/timeb.h>

/*-------------------------------------------------------------------
 * Name: CTimer()
 * Description:
 *   Constructor.  Initializes internal timer structures.
 *-----------------------------------------------------------------*/
CTimer::CTimer()
{
  QueryPerformanceFrequency((LARGE_INTEGER *)&m_TicksPerSecond); 
  m_BeginTime64 = 0;
  m_EndTime64 = 0;
  time(&m_LowResBeginTime);
  time(&m_LowResEndTime);
}

/*-------------------------------------------------------------------
 * Name: ~CTimer()
 * Description:
 *   Destructor.
 *-----------------------------------------------------------------*/
CTimer::~CTimer()
{
}

/*-------------------------------------------------------------------
 * Name: ResetTimer()
 * Description:
 *   Call this function to reset the high res timer.
 *-----------------------------------------------------------------*/
void CTimer::ResetTimer(void)
{
  QueryPerformanceCounter((LARGE_INTEGER *)&m_BeginTime64);
}

/*-------------------------------------------------------------------
 * Name: GetDeltaTime()
 * Description:
 *   Returns time elapsed since last call to ResetTimer()
 *   The high res timer is intended for short term timing measurements.
 *-----------------------------------------------------------------*/
double CTimer::GetDeltaTime(void)
{
  double delta_time;
  unsigned long elapsed_ticks, ticks_per_second;

  QueryPerformanceCounter((LARGE_INTEGER *)&m_EndTime64);
  elapsed_ticks = (unsigned long)(m_EndTime64 - m_BeginTime64);
  ticks_per_second = (unsigned long)m_TicksPerSecond;

  delta_time = (double)((m_EndTime64 - m_BeginTime64))/(double)ticks_per_second;

  return(delta_time);
}

/*-------------------------------------------------------------------
 * Name: Delay()
 * Description:
 *   Use for (very short) delays.
 *   WARNING - This function spins until the timeout, which uses
 *             a large amount of CPU time.  Use only for very short
 *             critical timing delays.  Even then, delay time may 
 *             be longer due to context switching.
 *-----------------------------------------------------------------*/
void CTimer::Delay(double time)
{
  __int64 num_ticks, end_time, current_time;

  num_ticks = (__int64)(m_TicksPerSecond*time);

  QueryPerformanceCounter((LARGE_INTEGER *)&current_time);

  end_time = current_time + num_ticks;
  
  while(current_time < end_time)
  {
    QueryPerformanceCounter((LARGE_INTEGER *)&current_time);
  }
}

/*-------------------------------------------------------------------
 * Name: GetElapsed()
 * Description:
 *   Returns elapsed time since the last call to GetElapsed().
 *   Call ResetTimer() before the first call.
 *-----------------------------------------------------------------*/
double CTimer::GetElapsed()
{
  double delta_time;
  unsigned long elapsed_ticks, ticks_per_second;

  QueryPerformanceCounter((LARGE_INTEGER *)&m_EndTime64);
  elapsed_ticks = (unsigned long)(m_EndTime64 - m_BeginTime64);
  ticks_per_second = (unsigned long)m_TicksPerSecond;

  delta_time = (double)((m_EndTime64 - m_BeginTime64))/(double)ticks_per_second;

  /* reset the time mark for next call */ 
  QueryPerformanceCounter((LARGE_INTEGER *)&m_BeginTime64);

  return(delta_time);
}

/*-------------------------------------------------------------------
 * Name: GetCurrentSystemTime()
 * Description:
 *   Returns current date/time as a CString.  Useful for timestamping
 *   log messages, etc.
 *-----------------------------------------------------------------*/
CString CTimer::GetCurrentSystemTime()
{
  CString sys_time;
  CString month;
  struct tm *utc_time;
  struct _timeb current_time;
  int tmp;
  int year;

  _ftime(&current_time);
  utc_time = gmtime(&(current_time.time));

  tmp = current_time.millitm;

  year = utc_time->tm_year + 1900;

  month = ConvertMonthToString(utc_time->tm_mon);

  sys_time.Format("%2d-%3s-%04d %02d:%02d:%02d.%03d", utc_time->tm_mday, month,
                  year, utc_time->tm_hour, utc_time->tm_min, utc_time->tm_sec,
                  current_time.millitm);  

  return(sys_time);
}

/*-------------------------------------------------------------------
 * Name: ResetLowResTimer()
 * Description:
 *   Call this to reset the low res timer.
 *
 *   For long term timers, use the Low Res timer, which is not
 *   limited like the 64-bit high res timer by overflow.
 *   The high res timer is intended for short term timing measurements.
 *-----------------------------------------------------------------*/
void CTimer::ResetLowResTimer()
{
  time(&m_LowResBeginTime);
}

/*-------------------------------------------------------------------
 * Name: GetLowResElapsed()
 * Description:
 *   Returns the elapsed time since ResetLowResTimer().
 *-----------------------------------------------------------------*/
double CTimer::GetLowResElapsed()
{
  double elapsed_time;

  time(&m_LowResEndTime);
  elapsed_time = difftime(m_LowResEndTime, m_LowResBeginTime);

  return(elapsed_time);
}

/*-------------------------------------------------------------------
 * Name: GetCurrentSystemTime()
 * Description:
 *   Returns data in pointers for the current system time.
 *-----------------------------------------------------------------*/
void CTimer::GetCurrentSystemTime(UINT &yr, UINT &month, UINT &day, 
                                  UINT &hour, UINT &min, float &sec)
{
  struct tm *utc_time;
  struct _timeb current_time;

  _ftime(&current_time);
  utc_time = gmtime(&(current_time.time));

  yr = utc_time->tm_year + 1900;
  month = utc_time->tm_mon + 1;
  day = utc_time->tm_mday;
  hour = utc_time->tm_hour;
  min = utc_time->tm_min;
  sec = (float)(utc_time->tm_sec + (current_time.millitm)/1000.0);
}

/*-------------------------------------------------------------------
 * Name: GetBaseFileName()
 * Description:
 *   Returns a base name for any file in three formats (see code).
 *-----------------------------------------------------------------*/
CString CTimer::GetBaseFileName(int nNameOptn)
{
  struct tm utc_time;
  CString sFileName, sTime, month;
  UINT year;

  /* Grab the Current System Time */
  GetCurrentSystemTime(utc_time);

  /* Perform conversions for easier use */
  year = utc_time.tm_year+1900;
  month = ConvertMonthToString(utc_time.tm_mon);

  /* Create the first part of the name dealing with the date */
  switch(nNameOptn)
  {
  case NAME_FORMAT_DAYFIRST:    //DDMMMYYYY_HHMMSS
    sFileName.Format("%02d%03s%04d", utc_time.tm_mday, month, year);
    break;
  case NAME_FORMAT_YEARFIRST:   //YYYYMMMDD_HHMMSS
    sFileName.Format("%04d%03s%02d", year, month, utc_time.tm_mday);
    break;
  case NAME_FORMAT_LONGDAY:     //YYYY_DDD_HHMMSS
    sFileName.Format("%04d_%03d", year, utc_time.tm_yday);
    break;
  default:
    sFileName = "";
    break;
  }

  /* Create the HHMMSS part of the name */
  sTime.Format("%02d%02d%02d", utc_time.tm_hour, utc_time.tm_min, utc_time.tm_sec);

  /* Final format */
  sFileName = sFileName + '_' + sTime;

  return(sFileName);
}

/*-------------------------------------------------------------------
 * Name: GetCurrentSystemTime()
 * Description:
 *   Overloaded public function to return a pointer to the time
 *   structure of the current UTC time.
 *-----------------------------------------------------------------*/
void CTimer::GetCurrentSystemTime(struct tm &time_struct)
{
  struct tm *utc_time;
  struct _timeb current_time;

  _ftime(&current_time);
  utc_time = gmtime(&(current_time.time));

  time_struct = *utc_time;
}

/*-------------------------------------------------------------------
 * Name: ConvertMonthToString()
 * Description:
 *   Public function to convert a UINT to a string for the Month of 
 *   the Year.  The input month starts with 0 for January.
 *-----------------------------------------------------------------*/
CString CTimer::ConvertMonthToString(UINT nMonth)
{
  CString month;

  /* Convert to string */
  switch(nMonth)
  {
  case 0:
    month = "JAN";
    break;
  case 1:
    month = "FEB";
    break;
  case 2:
    month = "MAR";
    break;
  case 3:
    month = "APR";
    break;
  case 4:
    month = "MAY";
    break;
  case 5:
    month = "JUN";
    break;
  case 6:
    month = "JUL";
    break;
  case 7:
    month = "AUG";
    break;
  case 8:
    month = "SEP";
    break;
  case 9:
    month = "OCT";
    break;
  case 10:
    month = "NOV";
    break;
  case 11:
    month = "DEC";
    break;
  }

  return(month);
}
