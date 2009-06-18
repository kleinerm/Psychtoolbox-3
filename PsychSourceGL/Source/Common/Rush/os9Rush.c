/* Rush.c

COPYRIGHT:
Copyright Denis Pelli, 1997. This file may be distributed freely as long
as this notice accompanies it and any changes are noted in the source. 
However, it may not be sold, in source or compiled form, without permission.
It is distributed as is, without any warranty implied or provided.  We
accept no liability for any damage or loss resulting from the use of
this software.

PURPOSE:
Run any critical user function with minimal interruption by Mac OS
system software. Without Rush, the Mac OS and device drivers steal
chunks of time whenever they like. E.g. the Zip disk driver, when no
disk is inserted, steals 2 ms once every 3 seconds. For a loop showing a
real-time movie, the possibility of losing 2 milliseconds, out of a
frame time of say 13 ms, substantially reduces the maximum number of
pixels that can be shown per frame if we insist on never missing a
frame. Rush implements two alternate solutions. Apple's guidelines
suggest that driver interrupt tasks should be very brief, passing any
lengthy tasks to the Deferred Task Manager to run later. When
priorityLevel is -1, Rush runs the user code as a deferred task, which
runs normally, except that all other deferred tasks are blocked until
our code finishes. When priorityLevel is 0 the user code runs normally.
When priorityLevel is >0 the user code runs at that raised processor
priority. The Deferred Task Manager doesn't run at all until the
processor priority goes back to zero. Thus any nonzero priorityLevel
will block all deferred tasks. A positive priorityLevel will also block
some primary interrupt tasks; more are blocked the higher the
priorityLevel. At priorityLevel 7 nearly all interrupts are blocked.

When priorityLevel is nonzero we block all the Mac's deferred tasks
while the user function is run. When priorityLevel is 1 (or more) we
block deferred tasks by raising the priority before calling the user
function. When priorityLevel is -1 we block deferred tasks by calling
the user function from within a deferred task.

The deferred-task solution is attractive. All deferred tasks are blocked
until ours finishes, and all urgent interrupt processes (including VBL
and Time Manager updates) occur normally. And keyboard and mouse still
work. This elegant solution was suggested by Bo3b Johnson at Apple
Developer Support on 4/19/97 (Follow-up:  418098). 

A slight drawback is that because the deferred task is interrupt driven,
we must save and restore the 68881 floating point registers (if we're
compiled TARGET_RT_MAC_68881) to allow the user's task to use floating
point. Apple says the save/restore takes about 50 times as long as a
MOVE. This overhead might lessen the attractiveness of the deferred-task
solution (priorityLevel -1) when running on 68K machines, but I suspect
that the overhead will be acceptable in most applications.

The raised-priority solution has a different drawback. Raising priority
(to 1 or more) does block the deferred tasks, but also freezes the mouse
and keyboard, and results in abnormal operation of the Time Manager
(overflow and coarse steps) and. The loss of the Time Manager usually
doesn't matter on PCI PowerMacs, because Seconds.c and WaitSeconds.c
then use the excellent UpTime routine, which is immune to priorityLevel.

4/22/97 Testing with Matlab LoopTest.m indicates that Rush.mex runs fine
at all priorities. However, a 2 ms interruption due to the Zip driver
(when no disk is inserted) occurs about one every three seconds at
priorities -1, 0, and 1, and only disappears at priorities 2 and higher.
Contrary to Apple's rules, the Zip driver's 2 ms interruption is NOT a
deferred task. We're hoping that Iomega will fix their driver soon. For
news you might look here:
<http://www.macintouch.com/jazprobs.html>

A THOUGHT ON THE 68881:
Hmm. I wonder. In fact we know for certain that we're interrupting
our own wait loop:
	while(!stuff.done) ;	// wait until our deferredTask is done
Neither this loop nor the function that it's in have any floating point,
so we know that the interrupt hasn't interrupted any floating-point
code. So the usual assumptions of a subroutine about the prior state of
the FPU are probably valid. The only exception that occurs to me is if
our wait loop is interrupted and that interrupt process is itself
interrupted. However, even in that case we'd only get into trouble if
the interrupted interrupt process was using the 68881 fpu, which is
extremely unlikely because Apple discourages it so strongly (because of
the overhead of saving & restoring the fpu). Thus, we could probably
safely comment out the calls to Save68881 and Restore68881, but I don't
want to do that without testing, and I have little incentive at this
point. If you find that the save/restore overhead is not negligible in
your application, then you may want to try that. If you confirm that
it's safe to ride without these training wheels, please let me know.

EXAMPLE 1:
Let's rush the calculation of c=a+b. (This simple example is solely for
the sake of exposition. I can't think of any reason to rush this. In
real life you'll only want to rush code that must synchronize with
external events, e.g. displaying or recording.)

typedef struct {
     int a,b,c;
} Abc;

void main(void)
{
     Abc abc;
     int priorityLevel=7;

     abc.a=a;
     abc.b=b;
     Rush(priorityLevel,&Summer,&abc);
     c=abc.c;
}

void Summer(void *argPtr)
{
     Abc *abcPtr;

     abcPtr=argPtr;
     abcPtr->c = abcPtr->a + abcPtr->b;
}

EXAMPLE 2:
This example illustrates something closer to the use for which Rush.c
was created. Here we call back to the main application (i.e. Matlab) to
rush high-level code. The C subroutine called "mexFunction" is a MEX
function, callable from Matlab programs, e.g. Rush('c=a+b'). The
mexFunction receives a string of Matlab code, e.g. "c=a+b". Our rushed C
routine, fun(), asks the Matlab application to use "eval" to execute the
string, which has the effect of rushing this bit of Matlab code. Again,
in real life you'll only want to rush code that must synchronize with
external events. In my research, we now Rush all our Matlab display
loops, to produce frame-accurate real-time movies. The call to
mexSetTrapFlag (a Matlab internal routine) is significant. It tells
Matlab, if it encounters an error, to nevertheless return here to the
caller. It would be bad for Matlab to print an error message and stop
without returning, because the priorityLevel may have frozen the
keyboard and mouse, and a deferred task will hang forever if it tries to
access the disk. As implemented here, Matlab always returns, even if it
encounters an error, so everything's hunky dory.

typedef struct{
	mxArray *mxString;
	long error;
} Stuff;

void mexFunction(int nlhs, mxArray *plhs[], int nrhs, CONSTmxArray *prhs[])
{
	Stuff stuff;
	
	stuff.mxString=prhs[0];
	stuff.error=0;
	Rush(priorityLevel,&fun,&stuff);
}

void fun(void *argPtr)
{
	Stuff *stuffPtr;
	mxArray *plhs[1],*prhs[1];
	
	stuffPtr=(Stuff *)argPtr;
	prhs[0]=stuffPtr->mxString;
	mexSetTrapFlag(1);
	stuffPtr->error=mexCallMATLAB(0,plhs,1,prhs,"eval");
	mexSetTrapFlag(0);
}

HISTORY:
4/21/97		dgp		Wrote it as a MEX file.
4/22/97		dgp		Polished the code. Marked the done flag as "volatile", 
					since it's set by the deferred task at interrupt time.
5/2/97		dgp 	Fixed bug in definition of GetA1 declaration to fix 68K crash reported by dhb.
6/22/97		dgp		Updated comments above.
7/1/97		dgp		As requested by Josh Solomon, removed the mex-specific stuff to leave this
					generic version that i'm adding to the VideoToolbox. Josh wants to use
					it to create a Rush facility in Mathematica, like the one I created in Matlab.
7/9/97		dgp		Now save and restore the 68K fpu registers (e.g. 68881) when necessary.
					This allows the user's task to safely use floating point. The
					saving and restoring is only necessary when TARGET_RT_MAC_68881 is true and the
					priorityLevel is -1. The need for this is explained in Apple Technote hw22.
					<http://developer.apple.com/technotes/hw/hw_22.html>
7/31/97		dgp		Polished comments. Added conditionals for Matlab, which avoid calling GetPriority, by maintaining
					our own copy of the priorityLevel. This is based on the impression I had, while debugging, that
					calling GetPriority occassionally crashed. I'm not sure that impression was right, but this approach
					worked well when I tested the previous incarnation of Rush, so i'm inclined to keep doing it this way.
					David mentioned that the loops were running a bit slow on 68K, so I'm disabling the 68881 saving, since
					I think it's superfluous because I know that the code being interrupted doesn't use floating point.
8/8/97		dhb		Force include of Psychtoolbox.h if Matlab is defined.
1/6/98		dgp		Include Psychtoolbox.h only if Matlab is true. (We need it for the call to SetPsychPriority.)
					Fixed illegal goof that generated compiler error
					when compiled with Matlab false (apparently I'd never tested that case before).
3/22/99		dgp		Alerted by an Apple Tech Note, we now save and restore the PowerPC FP registers (see SaveFP.c) before running
					our deferred task. <http://developer.apple.com/technotes/tn/tn1158.html> It is conceivable
					that this isn't necessary for a deferred task, but I suspect it is. 
12/9/99 	dgp 	Updated for Universal Headers 3.3.
KNOWN BUGS:
*/
#define NEED_TO_SAVE_FPU 0
#if MATLAB
	#include <Psychtoolbox.h>
#else
	#include <VideoToolbox.h>
#endif
#ifndef __PROCESSES__
	#include <Processes.h>	// ProcessSerialNumber
#endif
#ifndef __TRAPS__
	#include <Traps.h>		// _DTInstall
#endif
#if TARGET_CPU_68K
	/*
	Here we decide whether we're being compiled
	as an application or code resource.
	In a code resource, A4 is used as the global pointer.
	In an application, it's A5. 
	*/
	#ifndef __A5__
		// CW Pro 2 defines __A5__, but I don't know about earlier versions or other compilers.
		#define __A5__ !MATLAB
	#endif
	#pragma parameter __D0 GetA1()
	long GetA1(void)= 0x2009;	// MOVE.L A1,D0
	#pragma parameter __D0 GetA4()
	long GetA4(void)= 0x200C;	// MOVE.L A4,D0
	#pragma parameter __D0 GetA5()
	long GetA5(void)= 0x200D;	// MOVE.L A5,D0
	#if THINK_C
		#pragma parameter __D0 SetA4(__D0)
		long SetA4(long) = 0xC18C;	// EXG D0,A4
	#else
		long SetA4(long:__D0):__D0 = 0xC18C;	// EXG D0,A4
	#endif
#else
	#define GetA1()		0L
	#define GetA4()		0L
	#define GetA5()		0L
	#define SetA4(x)	0L
#endif

// Inline code to save and restore FPU on 68881 etc.
// This is needed if we've arrived where we are by an interrupt
// that may be interrupting a floating point calculation.
// See Apple's Technote HW22 "Cooperating with the coprocessor"
// <http://developer.apple.com/technotes/hw/hw_22.html>
void Save68881(void);
void Restore68881(void);
#if TARGET_RT_MAC_68881
	void Save68881(void)={0xF327,0xF227,0xE0FF};	// FSAVE -(A7)			; save FPU state
													// FMOVEM FP0-FP7,-(A7)	; save FPU regs
	void Restore68881(void)={0xF21F,0xD0FF,0xF35F};	// FMOVEM (A7)+,FP0-FP7	; restore FPU regs
													// FRESTORE (A7)+		; restore FPU state
#endif

typedef struct{
	ProcessSerialNumber psn;
	DeferredTask *deferredTaskPtr;
	volatile long A,error,failedAttempts;
	void (*functionPtr)(void *argPtr);
	void *argPtr;
	volatile psych_bool done;
	int priorityLevel;
} Stuff;
#if TARGET_CPU_PPC
	static void OurDeferredTask(register Stuff *stuffPtr);
#else
	static void OurDeferredTask(void);
#endif
static void CallFunction(Stuff *stuffPtr);

int Rush(int priorityLevel,void (*functionPtr)(void *argPtr),void *argPtr);

int Rush(int priorityLevel,void (*functionPtr)(void *argPtr),void *argPtr)
{
	static psych_bool firstTime=1,deferAvailable;
	static DeferredTaskUPP deferredTaskUPP;
	int oldPriority;
	DeferredTask deferredTask;
	int error;
	Stuff stuff;
	
	if(firstTime){
		deferAvailable=TrapAvailable(_DTInstall);
		deferredTaskUPP=NewDeferredTaskProc(OurDeferredTask);
		firstTime=0;
	}
	if(priorityLevel<-1 || priorityLevel>7)PrintfExit("%s: Illegal priorityLevel %d.",__FILE__,priorityLevel);
	error=GetCurrentProcess(&stuff.psn);
	stuff.functionPtr=functionPtr;
	stuff.argPtr=argPtr;
	stuff.done=0;
	stuff.error=0;
	stuff.failedAttempts=0;
	stuff.deferredTaskPtr=NULL;
	stuff.priorityLevel=priorityLevel;
	switch(priorityLevel){
	case -1:
		// Run user's function as a deferred task.
		if(!deferAvailable)PrintfExit("%s: Your System is too old: no Deferred Task Manager.",__FILE__);
		#if MATLAB
			oldPriority=GetPsychTable()->priority;
		#else
			oldPriority=GetPriority();
		#endif
		if(oldPriority>0){
			#if MATLAB
				SetPsychPriority(0);
			#else
				SetPriority(0);
			#endif
			PrintfExit("%s: Can't defer task while processor priority is above zero.",__FILE__);
		}
		#if !__A5__
			stuff.A=GetA4();
		#else
			stuff.A=GetA5();
		#endif
		stuff.deferredTaskPtr=&deferredTask;
		deferredTask.qType=dtQType;
		deferredTask.dtAddr=deferredTaskUPP;
		deferredTask.dtReserved=0;
		deferredTask.dtParam=(long)&stuff;
		error=DTInstall(stuff.deferredTaskPtr);
		if(error)PrintfExit("%s: DTInstall error %d.",__FILE__,error);
		while(!stuff.done) ;	// wait until our deferredTask is done
		break;
	case 0:
		// Run user's function normally. 
		CallFunction(&stuff);
		break;
	default:
		// Run user's function at raised processor priority.
		#if MATLAB
			oldPriority=GetPsychTable()->priority;
			SetPsychPriority(priorityLevel);
		#else
			oldPriority=GetPriority();
			SetPriority(priorityLevel);
		#endif
		CallFunction(&stuff);
		#if MATLAB
			SetPsychPriority(oldPriority);
		#else
			SetPriority(oldPriority);
		#endif
		break;
	}
	if(stuff.error==1234)
		PrintfExit("%s: user function never ran; always interrupted wrong process.",__FILE__);
	if(stuff.failedAttempts)
		printf("%s: WARNING: user function ran only after %ld attempts that interrupted other processes.\n",__FILE__,stuff.failedAttempts);
	return stuff.failedAttempts;
}

#if (THINK_C || THINK_CPLUS || SYMANTEC_C)
	#pragma options(!profile)	// it would be dangerous to call the profiler from here
	#pragma options(assign_registers,redundant_loads)
#endif
#if __MWERKS__ && __profile__
	#pragma profile off			// on 68k it would be dangerous to call the profiler from here
#endif

#if TARGET_CPU_PPC
	static void OurDeferredTask(register Stuff *stuffPtr)
	{
		FPSaveRec fpSave;

		SaveFP(&fpSave);	// Only for PowerPC
		CallFunction(stuffPtr);
		RestoreFP(&fpSave);
	}
#else
	// The saving/restoring of 68881 wouldn't work if OurDeferredTask had any C use of floating point, because
	// in that case the compiler would insert floating point instructions at the beginning of the routine,
	// inevitably BEFORE we attempt to save the state, defeating our purpose. It's ok for the CallFunction() or the
	// routine(s) that it calls to use floating point.
	static void OurDeferredTask(void)
	{
		Stuff *stuffPtr;
		long oldA;

		stuffPtr=(void *)GetA1();
		#if !__A5__
			oldA=SetA4(stuffPtr->A);
		#else
			oldA=SetA5(stuffPtr->A);
		#endif
		#if TARGET_RT_MAC_68881 && NEED_TO_SAVE_FPU
			Save68881();
		#endif
		CallFunction(stuffPtr);
		#if TARGET_RT_MAC_68881 && NEED_TO_SAVE_FPU
			Restore68881();
		#endif
		#if !__A5__
			SetA4(oldA);
		#else
			SetA5(oldA);
		#endif
	}
#endif

static void CallFunction(Stuff *stuffPtr)
{
	ProcessSerialNumber psn;
	psych_bool isOurs;
	int error;

	error=GetCurrentProcess(&psn);
	error=SameProcess(&stuffPtr->psn,&psn,&isOurs);	// compare psns.
	if(isOurs){
		// It's safe. Let's do it.
		(*stuffPtr->functionPtr)(stuffPtr->argPtr);
		stuffPtr->error=0;
		stuffPtr->done=1;
	}else{
		// Oops. We're interrupting some other process. It's unsafe to run now.
		// Let's lie low, and reinstall ourselves to try again later.
		stuffPtr->failedAttempts++;
		if(stuffPtr->failedAttempts<10 && stuffPtr->deferredTaskPtr!=NULL)
			stuffPtr->error=DTInstall(stuffPtr->deferredTaskPtr);
		else stuffPtr->error=1234;
		if(stuffPtr->error)stuffPtr->done=1;	// Can't do it; give up.
	}
}


#if 0
// This code is copied from Apple Technote HW22 "Cooperating with the Coprocessor"
// http://developer.apple.com/technotes/hw/hw_22.html
// DON'T try to run these. They're here solely to get a disassembly. The assembly
// code must be run as inline code, which the definitions at the top of this
// file accomplish, based on the THINK C disassemblies of these functions.
// THINK C knows floating point assembly directives; CodeWarrior 10 doesn't.

void Save68881(void);
void Restore68881(void);

void Save68881(void){
	asm{
		FSAVE		-(SP)			; save the FP state
		FMOVEM.X	FP0-FP7,-(SP)	; save the FP regs we use
	}
}

void Restore68881(void){
	asm {
		FMOVEM.X	(SP)+,FP0-FP7	; replace the FP regs we used
		FRESTORE	(SP)+			; restore the FP state
	}
}
#endif

/*
As disassembled by THINK C. Note that we got FMOVEM not FMOVEM.X. I suspect that
this difference doesn't matter here, but I don't really know. Maybe it's not a difference
at all.

Save68881:
00000000: F327               FSAVE     -(A7)
00000002: F227 E0FF          FMOVEM    FP0-FP7,-(A7)

Restore68881:
00000000: F21F D0FF          FMOVEM    (A7)+,FP0-FP7
00000004: F35F               FRESTORE  (A7)+
*/


