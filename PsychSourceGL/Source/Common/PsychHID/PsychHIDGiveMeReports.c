/*
	PsychToolbox/Source/Common/PsychHID/PsychHIDGiveMeReports.c		

	PROJECTS: PsychHID

	PLATFORMS:  OSX

	AUTHORS:
	denis.pelli@nyu.edu dgp

	HISTORY:
	4/7/05  dgp	Wrote it, based on PsychHIDGetReport.c

 */

#include "PsychHID.h"

static char useString[]= "[reports,err]=PsychHID('GiveMeReports',deviceNumber,[reportBytes])";
static char synopsisString[]= 
	"Return, as an output argument, all the saved reports from the connected USB HID device. "
	"\"deviceNumber\" specifies which device. "
	"If supplied, the optional \"reportBytes\" argument imposes a maximum length on each report; "
	"if necessary, reports will be shortened, but not lengthened. "
	"(This feature allows you to receive a report containing just the data you requested when "
	"the firmware insists on providing a fixed length report that may be longer than the valid data. "
	"\"reports\" is a struct array, with a struct for each report. "
	"\"reports(i).report\" is a uint8 vector. "
	"If your device uses reportID then the first byte of the report is the reportID. "
	"\"reports(i).device\" is the device number. "
	"\"reports(i).type\" is the reportType: 1=input, 2=output, 3=feature. "
	"\"reports(i).time\" is the GetSecs time at which it was received from the Mac OS. "
	"The returned value \"err.n\" is zero upon success and a nonzero error code upon failure, "
	"as spelled out by \"err.name\" and \"err.description\". "
	;
static char seeAlsoString[]="SetReport, GetReport, ReceiveReports, ReceiveReportsStop, GiveMeReports.";

PsychError GiveMeReports(int deviceIndex,int reportBytes); // PsychHIDReceiveReports.c

PsychError PSYCHHIDGiveMeReports(void) 
{
	long error=0;
	int deviceIndex;
	int reportBytes=1024;
	mxArray **outErr;
	pRecDevice device;

    PsychPushHelp(useString,synopsisString,seeAlsoString);
    if(PsychIsGiveHelp()){PsychGiveHelp();return(PsychError_none);};
    PsychErrorExit(PsychCapNumOutputArgs(2));
    PsychErrorExit(PsychCapNumInputArgs(2));
	PsychCopyInIntegerArg(1,TRUE,&deviceIndex);
	PsychCopyInIntegerArg(2,false,&reportBytes);

	PsychHIDVerifyInit();
    device=PsychHIDGetDeviceRecordPtrFromIndex(deviceIndex);
	if(!HIDIsValidDevice(device))PrintfExit("PsychHID:GiveMeReports: Invalid device.\n");

	// reports
	error=GiveMeReports(deviceIndex,reportBytes); // PsychHIDReceiveReports.c
	
	// err
	outErr=PsychGetOutArgMxPtr(2); // outErr==NULL if optional argument is absent.
	if(outErr!=NULL){
		const char *fieldNames[]={"n", "name", "description"};
		mxArray *fieldValue;
		char *name="",*description="";

		PsychHIDErrors(error,&name,&description); // Get error name and description, if available.
		*outErr=mxCreateStructMatrix(1,1,3,fieldNames);
		fieldValue=mxCreateString(name);
		mxSetField(*outErr,0,"name",fieldValue);
		fieldValue=mxCreateString(description);
		mxSetField(*outErr,0,"description",fieldValue);
		fieldValue=mxCreateDoubleMatrix(1,1,mxREAL);
		*mxGetPr(fieldValue)=(double)error;
		mxSetField(*outErr,0,"n",fieldValue);
	}
    return(PsychError_none);	
}

