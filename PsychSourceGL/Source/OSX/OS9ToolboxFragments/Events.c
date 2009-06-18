/*
  PsychToolbox3/Source/Common/GetChar/GetCharHelpers.c		
  
  PROJECTS: GetChar only.
  
  PLATFORMS:  Only OS X so far.  
  
  AUTHORS:
  David Brainard		dhb		
  Denis Pelli			dgp
  Allen.Ingling@nyu.edu		awi 
      
  HISTORY:
  6/21/03  awi		Created.  Borrowed from OS 9 Psychtoolbox and modified.
  


*/



#include "Events.h"



/*
ROUTINE: WaitTicks
PURPOSE:
  Wait ticks (60.15 Hz), then return.
*/
void WaitTicks(long ticks)
{
  ticks+=TickCount();
  while(TickCount()+30<ticks)if(CommandPeriod())PrintfExit("User typed cmd-. Exiting.");
  while(TickCount()<ticks) ;
  return;
}



/*
    WaitNextEventOrQuit()
    
    
*/
psych_bool WaitNextEventOrQuit(int mask,EventRecord *eventPtr,unsigned long sleep,RgnHandle mouseRgn)
{
	psych_bool wne;

	if(CommandPeriod()){
			PrintfExit("User typed cmd-. Exiting.");
	}
	wne=WaitNextEvent(mask,eventPtr,sleep,mouseRgn);
	if(wne && IsCommandPeriod(eventPtr)){
			PrintfExit("User typed cmd-. Exiting.");
	}
	return wne;
}


/*
    CommandPeriod()
    
    
*/
psych_bool CommandPeriod(void)
{
	EventRecord event;

	if(EventAvail(keyDownMask|autoKeyMask,&event) && IsCommandPeriod(&event)){
		GetNextEvent(keyDownMask|autoKeyMask,&event);	// consume the event.
		return 1;
	}else return 0;	
}



/*
    IsCommandPeriod()


*/
psych_bool IsCommandPeriod(EventRecord *eventPtr)
{
	psych_bool isCommandPeriod;

	isCommandPeriod=(eventPtr->what==keyDown || eventPtr->what==autoKey) 
		&& IsCmdChar(eventPtr,'.') 
		&& (eventPtr->modifiers & cmdKey);
	return isCommandPeriod;
}

