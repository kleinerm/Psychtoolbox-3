/*
 Source/Common/PsychHID/PsychHIDSetReport.c		
 
 PROJECTS: PsychHID
 
 PLATFORMS:  OSX 
 
 AUTHORS:
 denis.pelli@nyu.edu dgp
 
 HISTORY:
 3/15/05  dgp		Wrote it.
 4/23/05  dgp Now use low-level setReport instead of high-level HIDSetReport so that we can specify a time out (50 ms) instead of hanging forever when there is trouble.
 */

#include "PsychHID.h"

static char useString[]= "err=PsychHID('SetReport',deviceNumber,reportType,reportID,report)";
static char synopsisString[] = "Send a report to the connected USB HID device. \"deviceNumber\" specifies which device. "
"\"reportType\" is 1=input, 2=output, 3=feature (0 to just echo arguments). "
"\"reportID\" is either zero or an integer (1 to 255) specifying the topic, e.g. read analog, read digital, write analog, etc. "
"\"report\" must be an array of char or integer (8-, 16-, or 32-bit, signed or unsigned) holding "
"the correct total number of bytes. "
"The returned value \"err.n\" is zero upon success and a nonzero error code upon failure, "
"as spelled out by \"err.name\" and \"err.description\". "
"For your convenience, if reportID is nonzero, then the first byte of \"report\" will be set to reportID.";
static char seeAlsoString[] = "GetReport";

// Get a report from an HID device.
// prototype for routine in Apple's HID Utilities Source/HID_Queue_Utilities.c
long HIDSetReport(pRecDevice pDevice,const IOHIDReportType reportType, const UInt32 reportID, void* reportBuffer, const UInt32 reportBufferSize);

/*
 The HID firmware notes I have to program the Measurement Computing PMD1208FS indicate that the reportID is one byte. It specifies
 what the report is for (0x03 read digital, 0x04 write digital, 0x10 read analog, 0x14 write analog, etc.).
 */

extern double AInScanStart;

PsychError PSYCHHIDSetReport(void) 
{
	long error;
	pRecDevice device;
	int deviceIndex;
	int reportType; // kIOHIDReportTypeInput=0, kIOHIDReportTypeOutput=1, or kIOHIDReportTypeFeature=2
	int reportID;
	unsigned char *reportBuffer;
	int reportSize;
	const mxArray *report;
	mxArray **outErr;
	int i;
	
    PsychPushHelp(useString,synopsisString,seeAlsoString);
    if(PsychIsGiveHelp()){PsychGiveHelp();return(PsychError_none);};
    PsychErrorExit(PsychCapNumOutputArgs(1));
    PsychErrorExit(PsychCapNumInputArgs(4));
	outErr=PsychGetOutArgMxPtr(1);
	assert(outErr!=NULL);
	PsychCopyInIntegerArg(1,TRUE,&deviceIndex);
	PsychCopyInIntegerArg(2,TRUE,&reportType);
	PsychCopyInIntegerArg(3,TRUE,&reportID);
	report=PsychGetInArgMxPtr(4); 
	reportBuffer=(void *)mxGetData(report);
	assert(reportBuffer!=NULL);
#ifndef PTBOCTAVE
	switch(mxGetClassID(report)){
		case mxCHAR_CLASS:    
		case mxINT8_CLASS: 
		case mxUINT8_CLASS:   
		case mxINT16_CLASS:  
		case mxUINT16_CLASS:  
		case mxINT32_CLASS:  
		case mxUINT32_CLASS: 
			break;
		default:
			PrintfExit("\"report\" array must be char or integer (8-, 16-, or 32-bit).");
			break;
	}
#endif
	reportSize=mxGetElementSize(report)*mxGetNumberOfElements(report);
	PsychHIDVerifyInit();
    device=PsychHIDGetDeviceRecordPtrFromIndex(deviceIndex);
	if(reportSize>0 && reportID!=0) *reportBuffer=0xff&reportID; // copy low byte of reportID to first byte of report.
																 // Apple defines constants for the reportType with values (0,1,2) that are one less that those specified by USB (1,2,3).
	assert(kIOHIDReportTypeInput==0 && kIOHIDReportTypeOutput==1 && kIOHIDReportTypeFeature==2);
	if(reportType==0){
		printf("SetReport(reportType %d, reportID %d, report ",reportType,reportID);
		for(i=0;i<reportSize;i++)printf("%d ",(int)reportBuffer[i]);
		printf(")\n");
		error=0;
	}else{
		if(1){
			IOHIDDeviceInterface122 **interface;
			interface=(IOHIDDeviceInterface122 **)(device->interface);
			if(interface)error=(*interface)->setReport(interface,reportType-1,reportID,reportBuffer,(UInt32)reportSize,50,NULL,NULL,NULL);
			else PrintfExit("PsychHID SetReport: Bad interface.\n");
		}else error=HIDSetReport(device,reportType-1,reportID,reportBuffer,(UInt32)reportSize);
	}
	if(reportID==0x11){
		PsychGetPrecisionTimerSeconds(&AInScanStart);
	}
	//if(error)printf("Warning: PsychHID: HIDSetReport error %ld (0x%lx).",error,error);
	if(0){
		*outErr=mxCreateDoubleMatrix(1,1,mxREAL);
		if(*outErr==NULL)PrintfExit("Couldn't allocate \"err\".");
		*mxGetPr(*outErr)=(double)error;
	}else{
		const char *fieldNames[]={"n", "name", "description"};
		char *name="",*description="";
		mxArray *fieldValue;
		
		PsychHIDErrors(error,&name,&description);
		*outErr=mxCreateStructMatrix(1,1,3,fieldNames);
		fieldValue=mxCreateString(name);
		if(fieldValue==NULL)PrintfExit("Couldn't allocate \"err\".");
		mxSetField(*outErr,0,"name",fieldValue);
		fieldValue=mxCreateString(description);
		if(fieldValue==NULL)PrintfExit("Couldn't allocate \"err\".");
		mxSetField(*outErr,0,"description",fieldValue);
		fieldValue=mxCreateDoubleMatrix(1,1,mxREAL);
		if(fieldValue==NULL)PrintfExit("Couldn't allocate \"err\".");
		*mxGetPr(fieldValue)=(double)error;
		mxSetField(*outErr,0,"n",fieldValue);
	}		
    return(PsychError_none);	
}

