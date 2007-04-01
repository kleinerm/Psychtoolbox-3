/*
	PsychToolbox/Source/Common/PsychHID/PsychHIDReceiveReports.c		

	PROJECTS: PsychHID

	PLATFORMS:  OSX

	AUTHORS:
	denis.pelli@nyu.edu dgp

	HISTORY:
	4/7/05  dgp	Wrote it, based on PsychHIDGetReport.c

	READ:
	bugs in mac os x retrieval of reports.
	http://lists.apple.com/archives/usb/2004/Jul/msg00003.html
	$900 USB bus analyzer
	http://lists.apple.com/archives/usb/2003/Dec/msg00011.html
	guido kolber reports problem with getting port 
	http://lists.apple.com/archives/usb/2004/Nov/msg00062.html
	usb mailing list:
	http://lists.apple.com/mailman/listinfo/usb
	header file with helpful comments:
	http://darwinsource.opendarwin.org/10.3.6/IOHIDFamily-86.21/IOHIDLib/IOHIDDeviceClass.h
	my query on the Apple usb list:
	http://lists.apple.com/archives/usb/2005/Apr/msg00050.html
	HID documentation:
	http://developer.apple.com/documentation/DeviceDrivers/Conceptual/HID/
	How to set up notification callback when a device is attached
	or removed:
	file:///Developer/Examples/IOKit/usb/Another%20USB%20Notification%20Example/USBNotificationExample.c
	http://developer.apple.com/samplecode/USBPrivateDataSample/USBPrivateDataSample.html
 
	NOTE ON GETREPORT: 
	4 April 2005. Implementing SetReport, to send a report to the
	device, was straightforward, just a matter of calling Apple's HID
	Utilities function HIDSetReport. Implementing "GetReport", to
	receive a report from the device, was arduous. I quickly
	implemented a call to Apple's HIDGetReport, and it worked with my
	keyboard and mouse, but it never worked with my PMD-1208FS. The
	GetReport request always resulted in a "stall", indicating that the
	PMD-1208FS was rejecting the request. This baffled me. Last friday,
	the firmware engineer, Nick, who programmed the PMD-1208FS,
	explained to me that I shouldn't use the GetReport command because
	that goes out on the USB bus, making a request to the device, which
	is too slow a way of communicating, so the device doesn't support
	it. This surprised me. I had supposed until then that GetReport
	merely grabbed whatever report had already been received by the
	host. Following a suggestion of Allen Ingling's I looked again in
	Apple's HID Manager documentation and discovered that the command
	one should use is in fact setInterruptReportHandlerCallback.
	However, the text explaining what this does and how to call it is
	very skimpy, not enough for me to figure it out. Fortunately
	googling setInterruptReportHandlerCallback turned up several helpful
	discussions on apple's usb developer forum. Remarkably, last
	november, the CEO of Code Mercenaries, who make IOWarrior, presented
	an abbreviated listing of exactly the code I needed, about five
	lines, asking why it broke in Mac OS 10.3.6. With apple's answer I
	was able to finish writing my code. I then had to read about a Mach
	feature in Mac OS X called a CFRunLoop, since
	setInterruptReportHandlerCallback implements the callback by
	installing a "source" in that loop. After about a day's fiddling it
	now works.
 */

#include "HID_Utilities_External.h" // HIDGetReport
//#include "/usr/include/mach/mach_port.h" // mach_port_deallocate
//#include "IOHIDLib.h" // setInterruptReportHandlerCallback
#include <IOKit/HID/IOHIDLib.h>
#include "PsychHID.h"
void CheckRunLoopSource(int deviceIndex,char *caller,int line);

static char useString[]= "err=PsychHID('ReceiveReports',deviceNumber[,options])";
static char synopsisString[]= 
"Receive and save, internally, all reports from the specified USB HID device, now and forever more (unless stopped). "
"Some parameters are persistent. When you don't explicitly supply a value they retain whatever value they had last time. "
"They do have an initial default (built-in) that is present when PsychHID is first called after being CLEARed. "
"Non-persistent parameters have a fixed default that applies every time you call PsychHID. "
"The returned value \"err.n\" is zero upon success and a nonzero error code upon failure, "
"as spelled out by \"err.name\" and \"err.description\". "
"\"deviceNumber\" specifies which device. "
"\"options.print\" =1 (initial default 0) enables diagnostic printing of a summary of each report when our callback routine receives it. "	
"\"options.printCrashers\" =1 (initial default 0) enables diagnostic printing of the creation of the callback source and its addition to the CFRunLoop. "
"\"options.maxReports\" (initial default 10000) allocate space for at least this many reports, shared among all devices. "
"\"options.maxReportSize\" (initial default 64) allocate this many bytes per report. "
"Requesting values of maxReports or maxReportSize beyond that provided by the current allocation will result in freeing the current allocation, "
"losing all currently held reports, and reallocating as specified. "
"\"options.secs\" (initial default 0.010 s) is how long to allow the run loop to issue callbacks. Each callback transfers one received report. "
"The Mac OS receives reports all the time. "
"It has a small buffer capacity, discarding the oldest. When requested by PsychHID, the Mac OS tranfers reports to "
"PsychHID (for all devices for which ReceiveReports is still active) using callbacks from the CFRunLoop. "
"(A callback means that the OS calls a routine of ours at some later time. "
"A CFRunLoop is a special loop associated with each thread that is invoked when we request it.) "
"Thus reports are received from the OS only during your call to ReceiveReports or GetReport. (They share the same callback routine.) "
"So you may want call ReceiveReports several times. "
"Reports can be received from multiple devices during a single call to ReceiveReports. "
"Calling ReceiveReports enables callbacks (forever) for the incoming reports from that device; "
"call ReceiveReportsStop to halt acquisition of further reports for a device; "
" you can resume acquisition for a device by calling ReceiveReports again. "
"Call GiveMeReports to get all the received reports and empty PsychHID's internal store for a device. "
"PsychHID can hold up to options.maxReports reports (total among all devices), and discards new incoming reports when it has no room to hold them. "
"For prolonged data acquisition you may need to call GiveMeReports periodically, emptying PsychHID's store before it becomes full. "
"PsychHID was enhanced by adding HID commands to send and receive HID reports to support the PMD-1208FS. "
"PsychHID is likely to work with other HID-compliant USB devices as well. "
"The device-specific programming for the PMD-1208FS is entirely in the MATLAB M files of the Daq Toolbox; "
"what is specific to Mac OS X is entirely in PsychHID. "
"PsychHID is entirely generic, following the HID standard; none of the internal code is specific to any device, "
"but we tested it primarily with the PMD-1208FS, "
"as well as keyboards, mice, and gamepads, so working with new devices may be an adventure. "
;
static char seeAlsoString[]="SetReport, ReceiveReportsStop, GiveMeReports";

// internal prototypes
PsychError ReceiveReportsStop(int deviceIndex);  // function is below.
void ReportCallback(void *target,IOReturn result,void *refcon,void *sender,UInt32 bufferSize); // function is below.
void CountReports(char *string);

#define MAXREPORTSIZE 64
#define MAXDEVICEINDEXS 64
#define MAXREPORTS 10000

typedef struct ReportStruct{
	int deviceIndex;
	IOReturn error;
	UInt32 bytes;
	double time;
	//int type; // 1=input, 2=output, 3=feature
	struct ReportStruct *next;
	unsigned char report[MAXREPORTSIZE];
} ReportStruct;

// These are out here for easy access by several routines in this file.
static boolean             ready[MAXDEVICEINDEXS]; 
static CFRunLoopSourceRef source[MAXDEVICEINDEXS]; 
static boolean optionsPrintReportSummary=0;	// options.print: Enable diagnostic print of report by ReportCallback.
static boolean optionsPrintCrashers=0;		// options.printCrashers
static int optionsMaxReports=10000;			// options.maxReports
static int optionsMaxReportSize=64;			// options.maxReportSize
static double optionsSecs=0.010;			// options.secs
static CFStringRef myRunLoopMode=NULL;		// CFSTR("myMode");

// These are out here for easy access by my report callback function: ReportCallback.
static ReportStruct *freeReportsPtr=NULL,*deviceReportsPtr[MAXDEVICEINDEXS]; // linked list headers

// Set by PsychHIDSetReport, read by ReportCallback solely for the optionsPrintReportSummary.
double AInScanStart=0;

void CountReports(char *string)
{
	int i,n;
	int listLength[MAXDEVICEINDEXS];
	ReportStruct *r;
	static boolean reportsHaveBeenAllocated=0;

	if(myRunLoopMode==NULL)myRunLoopMode=CFSTR("myMode"); // kCFRunLoopDefaultMode
	if(!reportsHaveBeenAllocated){
		// initial set up. Allocate free reports.
		static ReportStruct allocatedReports[MAXREPORTS];
		
		reportsHaveBeenAllocated=1;
		freeReportsPtr=&(allocatedReports[0]);
		for(i=0;i<MAXREPORTS;i++){
			r=&(allocatedReports[i]);
			r->next=&(allocatedReports[i+1]);
		}
		r->next=NULL;
                // MK: Adding initialization of source and ready arrays.
                // Does this resolve the bugreports about crashes from the forum?
                // Messages of Ben Heasley and Maria McKinley, Bugid 5522?
		for(i=0;i<MAXDEVICEINDEXS;i++) {
                    deviceReportsPtr[i]=NULL;
                    source[i]=NULL;
                    ready[i]=0;
                }
	}
	
	n=0;
	for(i=0;i<MAXDEVICEINDEXS;i++){
		if(i==0)r=freeReportsPtr;
		else r=deviceReportsPtr[i];
		listLength[i]=0;
		while(r!=NULL){
			r=r->next;
			listLength[i]++;
		}
		n=n+listLength[i];
	}
	if(n!=MAXREPORTS){
		printf("%s",string);
		printf(" device:reports. free:%3d",listLength[0]);
		for(i=1;i<MAXDEVICEINDEXS;i++)if(listLength[i]>0)printf(", %2d:%3d",i,listLength[i]);
		printf("\n");
	}
}

void ReportCallback(void *target,IOReturn result,void *refcon,void *sender,UInt32 bufferSize)
{
	int deviceIndex,i,n,m;
	unsigned char *ptr;
	ReportStruct *r;
	
	CountReports("ReportCallback beginning.");
	
	deviceIndex=(int)refcon;
	if(deviceIndex<0 | deviceIndex>MAXDEVICEINDEXS-1){
		printf("ReportCallback received out-of-range deviceIndex %d. Substituting zero.\n",deviceIndex);
		deviceIndex=0;
	}
	
	// take report from free list.
	if(freeReportsPtr==NULL){
		// Darn. We're full. It might be elegant to discard oldest report, but for now, we'll just ignore the new one.
		printf("ReportCallback warning. No more free reports. Discarding new report.\n");
		return;
	}
	r=freeReportsPtr;
	freeReportsPtr=r->next;
	r->next=NULL;
	
	// install report into the device's list.
	r->next=deviceReportsPtr[deviceIndex];
	deviceReportsPtr[deviceIndex]=r;
	
	// fill in the rest of the report struct
	r->error=result;
	r->bytes=bufferSize;
	r->deviceIndex=deviceIndex;
	ptr=target;
	for(i=0;i<bufferSize && i<MAXREPORTSIZE;i++)r->report[i]=*(ptr+i);
	PsychGetPrecisionTimerSeconds(&r->time);
	if(optionsPrintReportSummary){
		// print diagnostic summary of the report
		int serial;
		
		serial=r->report[62]+256*r->report[63]; // 32-bit serial number at end of AInScan report from PMD-1208FS
		printf("Got input report %4d: %2ld bytes, dev. %d, %4.0f ms. ",serial,(long)r->bytes,deviceIndex,1000*(r->time-AInScanStart));
		if(r->bytes>0){
			printf(" report ");
			n=r->bytes;
			if(n>6)n=6;
			for(i=0;i<n;i++)printf("%3d ",(int)r->report[i]);
			m=r->bytes-2;
			if(m>i){
				printf("... ");
				i=m;
			}
			for(;i<r->bytes;i++)printf("%3d ",(int)r->report[i]);
		}
		printf("\n");
	}
	CountReports("ReportCallback end.");
	return;
}

void mexMakeMemoryPersistent(void *ptr);
#ifndef PTBOCTAVE
void mxFree(void *ptr);
void *mxCalloc(size_t n, size_t size);
#endif

PsychError PSYCHHIDReceiveReports(void) 
{
	long error=0;
	int deviceIndex;
	mxArray **mxErrPtr;
	const mxArray *mxOptions,*mx;

    PsychPushHelp(useString,synopsisString,seeAlsoString);
    if(PsychIsGiveHelp()){PsychGiveHelp();return(PsychError_none);};
    PsychErrorExit(PsychCapNumOutputArgs(1));
    PsychErrorExit(PsychCapNumInputArgs(2));
	PsychCopyInIntegerArg(1,TRUE,&deviceIndex);
	/*
	 "\"options.print\" =1 (default 0) enables diagnostic printing of a summary of each report when our callback routine receives it. "	
	 "\"options.printCrashers\" =1 (default 0) enables diagnostic printing of the creation of the callback source and its addition to the CFRunLoop. "
	 "\"options.maxReports\" (default 10000) allocate space for at least this many reports, shared among all devices. "
	 "\"options.maxReportSize\" (default 64) allocate this many bytes per report. "
	 */
	//optionsPrintReportSummary=0;	// options.print
	//optionsPrintCrashers=0;		// options.printCrashers
	//optionsMaxReports=10000;		// options.maxReports
	//optionsMaxReportSize=64;		// options.maxReportSize
	//optionsSecs=0.010;			// options.secs
	mxOptions=PsychGetInArgMxPtr(2);
	if(mxOptions!=NULL){
		mx=mxGetField(mxOptions,0,"print");
		if(mx!=NULL)optionsPrintReportSummary=(boolean)mxGetScalar(mx);
		mx=mxGetField(mxOptions,0,"printCrashers");
		if(mx!=NULL)optionsPrintCrashers=(boolean)mxGetScalar(mx);
		mx=mxGetField(mxOptions,0,"maxReports");
		if(mx!=NULL)optionsMaxReports=(int)mxGetScalar(mx);
		mx=mxGetField(mxOptions,0,"maxReportSize");
		if(mx!=NULL)optionsMaxReportSize=(int)mxGetScalar(mx);
		mx=mxGetField(mxOptions,0,"secs");
		if(mx!=NULL)optionsSecs=mxGetScalar(mx);
	}
	if(optionsMaxReports>MAXREPORTS)printf("PsychHID ReceiveReports: Sorry, maxReports is fixed at %d.\n",(int)MAXREPORTS);
	if(optionsMaxReportSize>MAXREPORTSIZE)printf("PsychHID ReceiveReports: Sorry, maxReportSize is fixed at %d.\n",(int)MAXREPORTSIZE);

	error=ReceiveReports(deviceIndex);

	mxErrPtr=PsychGetOutArgMxPtr(1);
	if(mxErrPtr!=NULL){
		const char *fieldNames[]={"n", "name", "description"};
		char *name="",*description="";
		mxArray *fieldValue;

		PsychHIDErrors(error,&name,&description); // Get error name and description, if available.
		*mxErrPtr=mxCreateStructMatrix(1,1,3,fieldNames);
		fieldValue=mxCreateString(name);
		if(fieldValue==NULL)PrintfExit("PSYCHHIDReceiveReports: Couldn't allocate \"err\".");
		mxSetField(*mxErrPtr,0,"name",fieldValue);
		fieldValue=mxCreateString(description);
		if(fieldValue==NULL)PrintfExit("PSYCHHIDReceiveReports: Couldn't allocate \"err\".");
		mxSetField(*mxErrPtr,0,"description",fieldValue);
		fieldValue=mxCreateDoubleMatrix(1,1,mxREAL);
		if(fieldValue==NULL)PrintfExit("PSYCHHIDReceiveReports: Couldn't allocate \"err\".");
		*mxGetPr(fieldValue)=(double)error;
		mxSetField(*mxErrPtr,0,"n",fieldValue);
	}
    return(PsychError_none);	
}

// GiveMeReports is called solely by PsychHIDGiveMeReports, but the code resides here
// in PsychHIDReceiveReports because it uses the typedefs and static variables that
// are defined solely in this file. The linked lists of reports are unknown outside of this file.
PsychError GiveMeReports(int deviceIndex,int reportBytes)
{
	long dims[]={1,1};
	mxArray **outReports;
	ReportStruct *r,*rTail;
	const char *fieldNames[]={"report", "device", "time"};
	mxArray *fieldValue;
	unsigned char *reportBuffer;
	int i,j,n;
	long error=0;
	double now;
	
	CountReports("GiveMeReports beginning.");

	outReports=PsychGetOutArgMxPtr(1); 
	r=deviceReportsPtr[deviceIndex];
	n=0;
	while(r!=NULL){
		n++;
		rTail=r;
		r=r->next;
	}
	*outReports=mxCreateStructMatrix(1,n,3,fieldNames);
	r=deviceReportsPtr[deviceIndex];
	PsychGetPrecisionTimerSeconds(&now);
	for(i=n-1;i>=0;i--){
		assert(r!=NULL);
		if(r->error)error=r->error;
		dims[0]=1;
		//printf("%2d: r->bytes %2d, reportBytes %4d, -%4.1f s\n",i,(int)r->bytes,(int)reportBytes, now-r->time);
		if(r->bytes>reportBytes)r->bytes=reportBytes;
		dims[1]=r->bytes;
		fieldValue=mxCreateNumericArray(2,(void *)dims,mxUINT8_CLASS,mxREAL);
		reportBuffer=(void *)mxGetData(fieldValue);
		for(j=0;j<r->bytes;j++)reportBuffer[j]=r->report[j];
		if(fieldValue==NULL)PrintfExit("Couldn't allocate report array.");
		mxSetField(*outReports,i,"report",fieldValue);
		fieldValue=mxCreateDoubleMatrix(1,1,mxREAL);
		*mxGetPr(fieldValue)=(double)r->deviceIndex;
		mxSetField(*outReports,i,"device",fieldValue);
		fieldValue=mxCreateDoubleMatrix(1,1,mxREAL);
		*mxGetPr(fieldValue)=r->time;
		mxSetField(*outReports,i,"time",fieldValue);
		r=r->next;
	}
	if(deviceReportsPtr[deviceIndex]!=NULL){
		// transfer all these now-obsolete reports to the free list
		rTail->next=freeReportsPtr;
		freeReportsPtr=deviceReportsPtr[deviceIndex];
		deviceReportsPtr[deviceIndex]=NULL;
	}
	CountReports("GiveMeReports end.");
	return error;
}


// Called solely by PsychHIDGetReport, but resides here in order to access the linked list of reports.
PsychError GiveMeReport(int deviceIndex,boolean *reportAvailablePtr,unsigned char *reportBuffer,UInt32 *reportBytesPtr,double *reportTimePtr)
{
	ReportStruct *r,*rOld;
	long error;
	int i;
	
	CountReports("GiveMeReport beginning.");

	r=deviceReportsPtr[deviceIndex];
	if(r!=NULL){ // report available?
				 // grab the oldest report for this device
		*reportAvailablePtr=1;
		if(r->next==NULL){
			deviceReportsPtr[deviceIndex]=NULL;
		}else{
			while(r->next->next!=NULL)r=r->next;
			rOld=r;
			r=r->next;
			rOld->next=NULL;
		}
		if(*reportBytesPtr > r->bytes)*reportBytesPtr=r->bytes;
		for(i=0;i<*reportBytesPtr;i++)reportBuffer[i]=r->report[i];
		*reportTimePtr=r->time;
		error=r->error;
		
		// add it to the free list
		r->next=freeReportsPtr;
		freeReportsPtr=r;
	}else{
		*reportAvailablePtr=0;
		*reportBytesPtr=0;
		*reportTimePtr=0.0/0.0;
		error=0;
	}
	CountReports("GiveMeReport end.");
	return error;
}				  

PsychError ReceiveReports(int deviceIndex)
{
	long error=0;
	pRecDevice device;
	IOHIDDeviceInterface122** interface=NULL;
	int reason; // kCFRunLoopRunFinished, kCFRunLoopRunStopped, kCFRunLoopRunTimedOut, kCFRunLoopRunHandledSource

	CountReports("ReceiveReports beginning.");
	if(freeReportsPtr==NULL)PrintfExit("No free reports.");

	PsychHIDVerifyInit();
	device=PsychHIDGetDeviceRecordPtrFromIndex(deviceIndex);
	if(!HIDIsValidDevice(device))PrintfExit("PsychHID: Invalid device.\n");
	interface=device->interface;
	if(interface==NULL)PrintfExit("PsychHID: No interface for device.\n");
	if(deviceIndex<0 || deviceIndex>MAXDEVICEINDEXS-1)PrintfExit("Sorry. Can't cope with deviceNumber %d (more than %d). Please tell denis.pelli@nyu.edu",deviceIndex,(int)MAXDEVICEINDEXS-1);
	CheckRunLoopSource(deviceIndex,"ReceiveReports",__LINE__);
	if(!ready[deviceIndex]){
		// setInterruptReportHandlerCallback
		static unsigned char buffer[MAXREPORTSIZE];
		UInt32 bufferSize=MAXREPORTSIZE;
		boolean createSource;

		createSource=(source[deviceIndex]==NULL);
		if(createSource){
			if(optionsPrintCrashers && createSource)printf("%d: createAsyncEventSource\n",deviceIndex);
			error=(*interface)->createAsyncEventSource(interface,&(source[deviceIndex]));
			if(error)PrintfExit("ReceiveReports - createAsyncEventSource error 0x%lx.",error);
			if(0 && optionsPrintCrashers && createSource)
				printf("%d: source %4.4lx validity %d, CFRunLoopContainsSource is %d.\n",deviceIndex,(unsigned long)source[deviceIndex]
					   ,CFRunLoopSourceIsValid(source[deviceIndex])
					   ,CFRunLoopContainsSource(CFRunLoopGetCurrent(),source[deviceIndex],myRunLoopMode));
		}
		if(optionsPrintCrashers && createSource)printf("%d: getAsyncEventSource\n",deviceIndex);
		CheckRunLoopSource(deviceIndex,"ReceiveReports",__LINE__);
		if(optionsPrintCrashers && createSource)printf("%d: CFRunLoopAddSource\n",deviceIndex);
		CFRunLoopAddSource(CFRunLoopGetCurrent(),source[deviceIndex],myRunLoopMode);
		if(0 && optionsPrintCrashers && createSource)printf("%d: source %4.4lx validity %d, CFRunLoopContainsSource is %d.\n",deviceIndex,(unsigned long)source[deviceIndex]
			   ,CFRunLoopSourceIsValid(source[deviceIndex])
			   ,CFRunLoopContainsSource(CFRunLoopGetCurrent(),source[deviceIndex],myRunLoopMode));
		ready[deviceIndex]=1;
		CheckRunLoopSource(deviceIndex,"ReceiveReports",__LINE__);
		if(optionsPrintCrashers && createSource)printf("%d: setInterruptReportHandlerCallback\n",deviceIndex);
		error=(*interface)->setInterruptReportHandlerCallback(interface,buffer,bufferSize,ReportCallback,buffer,(void *)deviceIndex);
		if(error)PrintfExit("ReceiveReports - setInterruptReportHandlerCallback error 0x%lx.",error);
		if(optionsPrintCrashers && createSource)printf("%d: CFRunLoopRunInMode.\n",deviceIndex);
	}
	//printf("%d: CFRunLoopRunInMode\n",deviceIndex);
	reason=CFRunLoopRunInMode(myRunLoopMode,optionsSecs,false);
	if(reason!=kCFRunLoopRunTimedOut && reason!=kCFRunLoopRunHandledSource){
		char *s;
		switch(reason){
			case kCFRunLoopRunFinished: s="kCFRunLoopRunFinished"; break;
			case kCFRunLoopRunStopped: s="kCFRunLoopRunStopped"; break;
			case kCFRunLoopRunTimedOut: s="kCFRunLoopRunTimedOut"; break;
			case kCFRunLoopRunHandledSource: s="kCFRunLoopRunHandledSource"; break;
			default: s="of unknown reason.";
		}
		printf("RunLoop ended at %.3f s because %s.\n",CFAbsoluteTimeGetCurrent()-AInScanStart,s);			
	}
	CountReports("ReceiveReports end.");
	return error;
}

PsychError ReceiveReportsStop(int deviceIndex)
{
	CheckRunLoopSource(deviceIndex,"ReceiveReportsStop",__LINE__);
	if(ready[deviceIndex]){
		// Rob Yepez, at Apple, suggested that it might be better to call CFRunLoopRemoveSource than CFRunLoopSourceInvalidate.
		// He's right. There's no problem in re-enabling the callback after using CFRunLoopRemoveSource.
		// The source remains valid and can be added again to the run loop. Don't create it again.
		if(myRunLoopMode==NULL)myRunLoopMode=CFSTR("myMode");; // kCFRunLoopDefaultMode
		if(0 && optionsPrintCrashers)printf("%d: CFRunLoopRemoveSource\n",deviceIndex);
		CFRunLoopRemoveSource(CFRunLoopGetCurrent(),source[deviceIndex],myRunLoopMode);// kCFRunLoopDefaultMode
		if(0 && optionsPrintCrashers)printf("%d: source %4.4lx validity %d, CFRunLoopContainsSource is %d.\n",deviceIndex,(unsigned long)source[deviceIndex]
			   ,CFRunLoopSourceIsValid(source[deviceIndex])
			   ,CFRunLoopContainsSource(CFRunLoopGetCurrent(),source[deviceIndex],myRunLoopMode));
		ready[deviceIndex]=0;
	}
	CheckRunLoopSource(deviceIndex,"ReceiveReportsStop",__LINE__);
	return 0;
}

/*
1. one must call CFRunLoopSourceInvalidate to kill the callback established by setInterruptReportHandlerCallback 
 before calling HIDReleaseDeviceList.

2. after calling setInterruptReportHandlerCallback to enable the callback and CFRunLoopSourceInvalidate to disable 
it, it is then impossible to re-enable the callback with that source. To later re-enable, simply remove the source, instead of
invalidating it. Once i've called CFRunLoopSourceInvalidate it appears that my only option is to release the interface by calling 
 HIDReleaseDeviceList and start over.
*/
PsychError PsychHIDReceiveReportsCleanup(void) 
{
// On a hunch, we now do both: remove the source and invalidate it. Alas, it makes no difference. I still get the CLEAR MEX crash.
	int deviceIndex;
	
	//printf("Clean up before PsychHID is flushed.\n");
	for(deviceIndex=0;deviceIndex<MAXDEVICEINDEXS;deviceIndex++) if(source[deviceIndex]!=NULL) {
                CheckRunLoopSource(deviceIndex,"PsychHIDReceiveReportsCleanup",__LINE__);
		CFRunLoopRemoveSource(CFRunLoopGetCurrent(),source[deviceIndex],myRunLoopMode);// kCFRunLoopDefaultMode
		if(0 && optionsPrintCrashers)printf("%d: source %4.4lx validity %d, CFRunLoopContainsSource is %d.\n",deviceIndex,(unsigned long)source[deviceIndex]
					,CFRunLoopSourceIsValid(source[deviceIndex])
					,CFRunLoopContainsSource(CFRunLoopGetCurrent(),source[deviceIndex],myRunLoopMode));
		if(optionsPrintCrashers)printf("%d: CFRunLoopSourceInvalidate\n",deviceIndex);
		CFRunLoopSourceInvalidate(source[deviceIndex]);
		if(optionsPrintCrashers)printf("%d: source %4.4lx validity %d, CFRunLoopContainsSource is %d.\n",deviceIndex,(unsigned long)source[deviceIndex]
					,CFRunLoopSourceIsValid(source[deviceIndex])
					,CFRunLoopContainsSource(CFRunLoopGetCurrent(),source[deviceIndex],myRunLoopMode));
		ready[deviceIndex]=0;
		CheckRunLoopSource(deviceIndex,"PsychHIDReceiveReportsCleanup",__LINE__);
		source[deviceIndex]=NULL;
	}			
	return 0;
}

void CheckRunLoopSource(int deviceIndex,char *caller,int line){
	CFRunLoopSourceRef currentSource;
	pRecDevice device;
	IOHIDDeviceInterface122** interface;
	
	device=PsychHIDGetDeviceRecordPtrFromIndex(deviceIndex);
	if(!HIDIsValidDevice(device))PrintfExit("PsychHID: Invalid device.\n");
	interface=device->interface;
	if(interface==NULL)PrintfExit("PsychHID: No interface for device.\n");
	currentSource=(*interface)->getAsyncEventSource(interface);
	if(source[deviceIndex] != currentSource)printf("%s (%d): source[%d] %4.4lx != current source %4.4lx.\n"
			,caller,line,(int)deviceIndex,(unsigned long)source[deviceIndex],(unsigned long)currentSource);

	if(ready[deviceIndex] && (source[deviceIndex]!=NULL) && !CFRunLoopContainsSource(CFRunLoopGetCurrent(),source[deviceIndex],myRunLoopMode))
		printf("%d: %s(%d): \"ready\" but source not in CFRunLoop.\n",(int)deviceIndex,caller,line);
	if(!ready[deviceIndex] && (source[deviceIndex]!=NULL) && CFRunLoopContainsSource(CFRunLoopGetCurrent(),source[deviceIndex],myRunLoopMode))
		printf("%d: %s(%d): \"!ready\" yet source is in CFRunLoop.\n",(int)deviceIndex,caller,line);
}


/*
file:///Developer/ADC%20Reference%20Library/documentation/Darwin/Reference/IOKit/IOHIDLib/Classes/IOHIDDeviceInterface122/CompositePage.html#//apple_ref/doc/compositePage/c/func/setInterruptReportHandlerCallback
 
IOHIDDeviceInterface::
 
 IOHIDReportCallbackFunction

typedef void (*IOHIDReportCallbackFunction) (
											 void * target, 
											 IOReturn result, 
											 void * refcon, 
											 void * sender, 
											 UInt32 bufferSize); 


Type and arguments of callout C function that is used when a completion routine is called, see IOHIDLib.h:setReport().


Parameter Descriptions

target
void * pointer to your data, often a pointer to an object.

result
Completion result of desired operation.

refcon
void * pointer to more data.

sender
Interface instance sending the completion routine.

bufferSize
Size of the buffer received upon completion.
*/

