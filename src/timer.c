/*
 *  See License file
 */


#include <string.h>
#include "icelib.h"
#include "icelib_intern.h"


void
ICELIB_timerConstructor(ICELIB_TIMER* timer,
                        unsigned int  tickIntervalMS)
{
  memset( timer, 0, sizeof(*timer) );
  timer->tickIntervalMS = tickIntervalMS;
  timer->countUpMS      = 0;
  timer->timerState     = ICELIB_timerStopped;
}


void
ICELIB_timerStart(ICELIB_TIMER* timer,
                  unsigned int  timeoutMS)
{
  timer->timeoutValueMS = timeoutMS;
  timer->countUpMS      = 0;
  timer->timerState     = ICELIB_timerRunning;
}


void
ICELIB_timerStop(ICELIB_TIMER* timer)
{
  timer->timerState = ICELIB_timerStopped;
}


void
ICELIB_timerTick(ICELIB_TIMER* timer)
{
  if (timer->timerState == ICELIB_timerRunning)
  {

    timer->countUpMS += timer->tickIntervalMS;

    if (timer->countUpMS >= timer->timeoutValueMS)
    {
      timer->timerState = ICELIB_timerTimeout;
    }
  }
}


bool
ICELIB_timerIsRunning(const ICELIB_TIMER* timer)
{
  return timer->timerState == ICELIB_timerRunning;
}


bool
ICELIB_timerIsTimedOut(const ICELIB_TIMER* timer)
{
  return timer->timerState == ICELIB_timerTimeout;
}
