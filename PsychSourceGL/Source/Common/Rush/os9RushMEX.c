/* RushMEX.c

COPYRIGHT:
Copyright Denis Pelli, 1997-9. This file may be distributed freely as long
as this notice accompanies it and any changes are noted in the source. 
It is distributed as is, without any warranty implied or provided.  We
accept no liability for any damage or loss resulting from the use of
this software.

PURPOSE:

priorityLevel=MaxPriority(window,'WaitBlanking','CopyWindow');
loop={
	'for i=1:100;'
		'Screen(window,''WaitBlanking'');'
		'Screen(''CopyWindow'',image(i),window);'
	'end;'
};
Rush(loop,priorityLevel);

Uses the VideoToolbox Rush.c to run a critical bit of Matlab code with
minimal interruption by Mac OS system software. Without Rush, the Mac OS
and device drivers steal chunks of time whenever they like. E.g. the Zip
disk driver, when no disk is inserted, steals 2 ms once every 3 seconds.
For a loop showing a real-time movie, the possibility of losing 2
milliseconds, out of a frame time of say 13 ms, substantially reduces
the maximum number of pixels that can be shown per frame if we insist on
never missing a frame. Rush implements two alternate solutions. Apple's
guidelines suggest that driver interrupt tasks should be very brief,
passing any lengthy tasks to the Deferred Task Manager to run later.
When priorityLevel is 0.5 (formerly -1), Rush runs the user code as a deferred task,
which runs normally, except that all other deferred tasks are blocked
until our code finishes. When priorityLevel is 0 the user code runs
normally. When priorityLevel is 1 ... 7 the user code runs at that raised
processor priority. The Deferred Task Manager doesn't run at all until
the processor priority goes back to zero. Thus any nonzero priorityLevel
will block all deferred tasks. A priorityLevel>0.5 will also block
some primary interrupt tasks; more are blocked the higher the
priorityLevel. At priorityLevel 7 nearly all interrupts are blocked.

The code string received from the user is passed to Matlab's EVAL
command. If the user supplies an array or a cell array of
strings, then the concatenation of all the strings
is passed to EVAL. This is achieved by calling CELLSTR to make
it a cell array of strings, and CAT(2,cellArray{:}) to contenate them
into one string.

We trap Matlab errors while EVAL is running, by means of EVAL's catch
mechanism. When priorityLevel is nonzero we also block all the
Mac's deferred tasks while EVAL is running. Trapping errors allows us to
always restore things back to normal before returning control to the
user. When priorityLevel is 1 (or more) we block deferred tasks by
raising the priority before calling EVAL. When priorityLevel is 0.5 we
block deferred tasks by calling EVAL from within a deferred task.

The deferred-task solution is attractive. All deferred tasks are blocked
until ours finishes, and all urgent interrupt processes (including blanking
and Time Manager updates) occur normally. And keyboard and mouse still
work. This elegant solution was suggested by Bo3b Johnson at Apple
Developer Support on 4/19/97 (Follow-up:  418098).

A slight drawback is that because the deferred task is interrupt driven,
we must save and restore the 68881 floating point registers (if we're
compiled TARGET_RT_MAC_68881) to allow the user's task to use floating
point. Apple says the save/restore takes about 50 times as long as a
MOVE. This overhead might lessen the attractiveness of the deferred-task
solution (priorityLevel 0.5) when running on 68K machines, but I suspect
that the overhead will be acceptable in most applications.

The raised-priority solution has a different drawback. Raising priority
(to 1 or more) does block the deferred tasks, but also freezes the mouse
and keyboard, and results in abnormal operation of the Time Manager
(overflow and coarse steps) and. The loss of the Time Manager usually
doesn't matter on PCI PowerMacs, because Seconds.c and WaitSeconds.c
then use the excellent UpTime routine, which is immune to priorityLevel.

4/22/97 My latest testing, with LoopTest.m, indicates that Rush.mex runs
fine at all priorities. However, a 2 ms interruption due to the Zip
driver (when no disk is inserted) occurs about one every three seconds
at priorities 0, 0.5, and 1, and only disappears at priorities 2 and
higher. Contrary to Apple's rules, the Zip driver's 2 ms interruption is
NOT a deferred task. We're hoping that Iomega will fix their driver
soon. For news you might look here:
<http://www.macintouch.com/jazprobs.html>

Rush.mex is reentrant, e.g. this works: Rush('Rush(''3*4'')'). However,
I can't think of any reason you'd want to do that.

NOTE: You can eliminate some interrupts by stuffing all apertures: keep a disk
in every disk drive (floppy, Zip, Jaz, etc.), because the annoying interrupts
turn out to be just to check whether you've inserted a disk. I don't know
if this applies to CD-ROM as well. I'd guess that the tray mechanism might
make the idle scanning unnecessary.

HISTORY:
7/1/97		dgp		Extracted this MEX-specific stuff from Rush.c, which now 
					resides in VideoToolbox.
7/9/97		dgp		Removed the 68881 restriction, subsequent to dealing with
					the issue in Rush.c.
2/1/98		dgp		Report any error that occurred during execution of the rushed code.
					This was much harder to implement than I expected because
					ERROR and error trapping don't work in the way I expected from the
					documentation. All the calls to ERROR in my callbacks to Matlab
					are ignored. The mexSetTrapFlag(1) feature does cause Matlab to return
					control to the caller if an error is detected, but fails to suppress
					the printing of the error message. I reported both anomalies to
					MathWorks as bugs in Matlab 5.2b1.
2/8/98		dgp		priorityLevel -1 renamed 0.5.
3/24/98		dgp		disallow priorityLevel "-1". Remove alternate calling form from the
					synopsis. (It's still in the M file help.)
7/16/98		dgp		Concatenate array or cell arrays of strings.
7/22/98		dgp		Issue error if Backgrounding is enabled. I think that Backgrounding would allow Matlab
					to share time with other Mac processes, while processor priority is raised. This would 
					result in a hang if they wait for an interrupt (eg try to access the disk). 
3/18/99		dgp		If possible, turn off Backgrounding during Rush, and then restore it.
11/15/99	dgp		Changed "#ifdef __PSYCHMAC__" to "#if TARGET_OS_MAC".
12/11/99	dgp		Updated for Universal Headers 3.3.

KNOWN BUGS:
*/
#include <Psychtoolbox.h>
int Rush(int priorityLevel,void (*functionPtr)(void *argPtr),void *argPtr);
typedef struct{
	long getTimes,iterations,error;
	CONSTmxArray *mxString;
	mxArray *mxTimes;
	psych_bool trapErrors;
} Stuff;
static void fun(void *argPtr);
static int CallMATLAB(CONSTmxArray *mxString,psych_bool trapErrors);

/*
ROUTINE: mexFunction()
*/
char useRush[]="Rush(string,[priorityLevel])";
// times=Rush(numberOfSamples,[priorityLevel])	% just for our testing, not published.

void mexFunction(int nlhs, mxArray *plhs[], int nrhs, CONSTmxArray *prhs[])
{
	static psych_bool firstTime=1,deferAvailable;
	long priorityLevel=-1;
	double a;
	Stuff stuff;
	psych_bool isString,oldMatlabBackgrounding;

	plhs;
	InitMEX();
	isString=nrhs>0 && (mxIsChar(prhs[0]) || mxIsCell(prhs[0]));
	if(nlhs>1 || nrhs<1 || nrhs>2 || (isString && nlhs>0))PrintfExit("Usage:\n\n%s",useRush);
	switch(nrhs){
	case 2:
		a=mxGetScalar(prhs[1]);
		if(a!=0 && a!=0.5 && a!=1 && a!=2 && a!=3 && a!=4 && a!=5 && a!=6 && a!=7)
			PrintfExit("Illegal priorityLevel %.1f. Must be 0, 0.5, 1, 2, 3, 4, 5, 6, or 7. Usage:\n%s",a,useRush);
		priorityLevel=a;
		if(a==0.5)priorityLevel=-1;
	case 1:
		if(!isString){
			// special case: get an array of timings
			double s,s0;
			long j;

			stuff.getTimes=mxGetScalar(prhs[0]);
			if(stuff.getTimes<1)
				PrintfExit("The first argument must be either a string (or strings) or a number.");
			plhs[0]=mxCreateDoubleMatrix(1,stuff.getTimes,mxREAL);
			if(plhs[0]==NULL)PrintfExit("Couldn't allocate %ld element array.",stuff.getTimes);
			stuff.mxTimes=plhs[0];
			stuff.iterations=10000;
			s=Seconds();
			s=Seconds();
			s0=Seconds()-s;
			s=Seconds();
			for(j=0;j<stuff.iterations;j++) ;
			s=Seconds()-s-s0;
			stuff.iterations=(0.001-s0)*stuff.iterations/s;
			// That many iterations plus a call to Seconds() should take 1 ms.
		}else stuff.getTimes=0;
		break;
	}
	if(1){
		// concatenate array or cell array of strings
		int error=0;
		mxArray *out,*in;

		in=(mxArray *)prhs[0];
		error=mexCallMATLAB(1,&out,1,&in,"CatStr"); // Psychtoolbox:PsychOneliners:CatStr.m
		stuff.mxString=out;
	}else{
		stuff.mxString=prhs[0];
	}
	stuff.trapErrors=1;
	stuff.error=0;
	
	#if TARGET_OS_MAC
		if(CanControlMatlabBackgrounding()){
			oldMatlabBackgrounding=MatlabBackgrounding(0);
		}else{
			if(firstTime){
				// Backgrounding is dangerous. Matlab would give time to another process while priority is raised.
				// Checking the enabled flag (on the disk) is slow, so we only do it once if backgrounding's off. But
				// if it's on, we give an error, and check again next time.
				if(IsMatlabBackgroundingEnabled()){
					PrintfExit("Rush: please turn off Matlab's \"Backgrounding\" Preference, in the File menu.\n");
				}else firstTime=0;
			}
		}
	#endif

	mexEvalString("lasterr('');");

	Rush(priorityLevel,&fun,&stuff);

	if(CanControlMatlabBackgrounding())MatlabBackgrounding(oldMatlabBackgrounding);
	if(stuff.error)mexErrMsgTxt("mexCallMATLAB error during Rush");	// never happens
	{
		// if there's a message in LASTERR, print it as an error, with a preface
		int nlhs=1,nrhs=0,error=0;
		mxArray *plhs[1]={NULL},*prhs[1]={NULL};
		char string[512]="Error during Rush: ",*errorMsg;

		errorMsg=string+strlen(string);
		error=mexCallMATLAB(nlhs,plhs,nrhs,prhs,"lasterr");
		error=mxGetString(plhs[0],errorMsg,sizeof(string)-strlen(string));
		if(strlen(errorMsg)>0)mexErrMsgTxt(string);
	}
}

#if (THINK_C || THINK_CPLUS || SYMANTEC_C)
	#pragma options(!profile)	// it would be dangerous to call the profiler from here
	#pragma options(assign_registers,redundant_loads)
#endif
#if __MWERKS__ && __profile__
	#pragma profile off			// on 68k it would be dangerous to call the profiler from here
#endif

static void fun(void *argPtr)
{
	long i,j;
	Stuff *stuffPtr=(Stuff *)argPtr;
	
	if(stuffPtr->getTimes){
		// Record times at a nominal rate of once per millisecond.
		for(i=0;i<stuffPtr->getTimes;i++){
			mxGetPr(stuffPtr->mxTimes)[i]=Seconds();
			for(j=0;j<stuffPtr->iterations;j++) ;	// dilly dally.
		}
	}else{
		stuffPtr->error=CallMATLAB(stuffPtr->mxString,stuffPtr->trapErrors);
	}
}

static int CallMATLAB(CONSTmxArray *mxString,psych_bool trapErrors)
{
	int nlhs=0,nrhs,error=0;
	mxArray *plhs[1]={NULL},*prhs[2]={NULL,NULL};

	/*
	It turns out to be easier to use the "catch" feature of EVAL instead of the
	error trap mechanism offered by mexSetTrapFlag. mexSetTrapFlag(1) is supposed
	to cause all errors to be trapped, leaving error handling to us, but in fact
	it fails to suppress the error message. Tested in Matlab 5.2b1.
	*/
	prhs[0]=(mxArray *)mxString;
	prhs[1]=mxCreateString(";");
	if(trapErrors)nrhs=2;
	else nrhs=1;
	error=mexCallMATLAB(nlhs,plhs,nrhs,prhs,"eval");
	mxDestroyArray(prhs[1]);
	return error;
}

