/*
	PsychToolbox/Source/Common/PsychHID/PsychHIDReceiveReportsStop.c		

	PROJECTS: PsychHID

	PLATFORMS:  All

	AUTHORS:
	denis.pelli@nyu.edu                 dgp
    mario.kleiner@tuebingen.mpg.de      mk

	HISTORY:
	4/7/05  dgp	Wrote it, based on PsychHIDGetReport.c

 */

#include "PsychHID.h"

static char useString[]= "err=PsychHID('ReceiveReportsStop',deviceNumber)";
static char synopsisString[]= 
	"Stop receiving and saving reports from the specified USB HID device.\n"
	"Calling ReceiveReports enables callbacks (forever) for the incoming reports from that device; "
	"call ReceiveReportsStop to halt acquisition of further reports for this device; "
	"you can resume acquisition by calling ReceiveReports again. "
	"Call GiveMeReports to get all the received reports and empty PsychHID's internal store for that device. "
	"\"deviceNumber\" specifies which device. "
	"The returned value \"err.n\" is zero upon success and a nonzero error code upon failure, "
	"as spelled out by \"err.name\" and \"err.description\". ";

static char seeAlsoString[]="SetReport, ReceiveReports, GiveMeReports";

PsychError PSYCHHIDReceiveReportsStop(void) 
{
	long error=0;
	int deviceIndex;
	mxArray **outErr;

    PsychPushHelp(useString,synopsisString,seeAlsoString);
    if(PsychIsGiveHelp()){PsychGiveHelp();return(PsychError_none);};
    PsychErrorExit(PsychCapNumOutputArgs(1));
    PsychErrorExit(PsychCapNumInputArgs(1));
	PsychCopyInIntegerArg(1,TRUE,&deviceIndex);
	
	error=ReceiveReportsStop(deviceIndex);

	outErr=PsychGetOutArgMxPtr(1);
	if(outErr!=NULL){
		char *name="",*description="";
		const char *fieldNames[]={"n", "name", "description"};
		mxArray *fieldValue;

		PsychHIDErrors(NULL, error,&name,&description); // Get error name and description, if available.
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
