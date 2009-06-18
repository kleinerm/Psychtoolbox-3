/*
  PsychToolbox3/Source/OSX/OS9ToolboxFragments/Events.h		
  
  PROJECTS: SysBeep, Getchar.
  
  PLATFORMS:  Only OS X.  
  
  AUTHORS:
  David Brainard		dhb		
  Denis Pelli			dgp
  Allen.Ingling@nyu.edu		awi 
      
  HISTORY:
  6/21/03  awi		Created.  
  


*/

#include "Psych.h"



/*
ROUTINE: WaitTicks
PURPOSE:
  Wait ticks (60.15 Hz), then return.
*/

void WaitTicks(long ticks);
psych_bool WaitNextEventOrQuit(int mask,EventRecord *eventPtr,unsigned long sleep,RgnHandle mouseRgn);
psych_bool CommandPeriod(void);
psych_bool IsCommandPeriod(EventRecord *eventPtr);

