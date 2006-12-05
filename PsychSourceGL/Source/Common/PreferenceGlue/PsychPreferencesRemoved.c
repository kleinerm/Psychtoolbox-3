/*

  	Psychtoolbox3/Source/Common/PreferenceGlue/PsychPreferences.c

  

	AUTHORS:

  	Allen.Ingling@nyu.edu		awi 

  

	PLATFORMS: MATLAB only

  

  	PROJECTS:

  	8/04/03	awi		PsychPreferences on OS X

   



  	HISTORY:

  	10/08/02  awi		Wrote it.  

  

  	DESCRIPTION:

	

	All Preferences state is now maintained in MATLAB preferences settings.  Screen preferences still exist but serve only as a conduit to MATLAB Psychtoolbox preferences.

	

	Because this is all MATLAB-specific there is no point in abstracting out the mex/mx api in the functions within this file.

  	

	TO DO:

	 

*/





#include "Psych.h"





Boolean IsMATLABMatrixTrue(mxArray *mptr)

{

	return(mxIsLogicalScalarTrue(mptr) || mxIsDouble(mptr) && (mxGetM(mptr)*mxGetN(mptr)==1)  && mxGetPr(mptr)[0]);

}





Boolean IsPsychPreferencesValid(void)

{

	mxArray		*mexCallInput, *mexCallOutput;

	

	mexCallInput=NULL;

	mexCallMATLAB(1,&mexCallOutput, 0, &mexCallInput, "IsPsychPreferencesValid");

	return(IsMATLABMatrixTrue(mexCallOutput));

}









/* 

	PreparePsychPreferences()

	

	It is harmless to call PreparePsychPreferences() if preferences are already intialized because it checks to see if they have been intialized 

	before it does anything.

	

	Initialize the Psychtoolbox preferences if they are not.  This should be called once at the head of every screen subfunction which accesses 

	preferences so that we do not attempt to access and use preference values which do not exist.

	

	Return a value indicating whether preferences were TRUE: intialized on the call function or FALSE had alread been initialized.



*/

Boolean PreparePsychPreferences(char *omitMexFunction)

{

	int			mexCallError;

	mxArray		*mexCallResult;

	mxArray		*mexCallInputs[1];

	int			numArguments;



	if(!IsPsychPreferencesValid()){

		//Have the m scripts do their work to set declared and intrinsic preferences

		mexCallResult=NULL;

		mexCallInputs[0]=mxCreateString(omitMexFunction);

		numArguments= (omitMexFunction==NULL) ? 0 : 1;

		mexCallError= mexCallMATLAB(0, &mexCallResult, 1, mexCallInputs, "CreatePsychIntrinsicPreferences");

		return(TRUE);

	}else

		return(FALSE);

}





/*

	DoesPsychPreferenceExist()

*/

Boolean DoesPsychPreferenceExist(char *flagName)

{

	int			mexCallError;

	mxArray		*mexCallInputs[2];

	mxArray		*mexCallOutput;



	mexCallInputs[0]=mxCreateString("Psychtoolbox");

	mexCallInputs[1]=mxCreateString(flagName);

	



	//check to see that the specified preference actually exists.

	mexCallError= mexCallMATLAB(1, &mexCallOutput, 2, mexCallInputs, "ispref");

	if(mexCallError)

		PsychErrorExitMsg(PsychError_internal,"Unexpected Error Calling MATLAB");

	return(IsMATLABMatrixTrue(mexCallOutput));

}







/*

	GetPreferencesFlag()

	

*/

Boolean GetPreferencesFlag(char *flagName)

{

	Boolean		preferenceExists;

	int			mexCallError;

	mxArray		*mexCallOutput;

	mxArray		*mexCallInputs[2];

	

	preferenceExists=DoesPsychPreferenceExist(flagName);

	if(!preferenceExists)

		PsychErrorExitMsg(PsychError_user,"Unknown Psychtoolbox preference specified");

	mexCallInputs[0]=mxCreateString("Psychtoolbox");

	mexCallInputs[1]=mxCreateString(flagName);

	mexCallError= mexCallMATLAB(1, &mexCallOutput, 2, mexCallInputs, "getpref");

	return(IsMATLABMatrixTrue(mexCallOutput));

}





/*

	SetPreferencesDoubleValue()

	

	Create the preference if it does not exist

	

*/

void SetPreferencesDoubleValue(char *flagName, double value )

{

	int			mexCallError;

	mxArray		*mexCallOutput;

	mxArray		*mexCallInputs[3];

	

	mexCallInputs[0]=mxCreateString("Psychtoolbox");

	mexCallInputs[1]=mxCreateString(flagName);

	mexCallInputs[2]=mxCreateDoubleMatrix(1,1,mxREAL);

	mxGetPr(mexCallInputs[2])[0]=value;

	mexCallError= mexCallMATLAB(0, &mexCallOutput, 3, mexCallInputs, "setpref");

}





/*

	SetPreferencesNativeDoublesArray()

	

	Create the preference if it does not exist

	

*/

void PsychSetPreferencesNativeDoublesArray(char *flagName, mxArray *setMat )

{

	int			mexCallError;

	mxArray		*mexCallOutput;

	mxArray		*mexCallInputs[3];

	

	mexCallInputs[0]=mxCreateString("Psychtoolbox");

	mexCallInputs[1]=mxCreateString(flagName);

	mexCallInputs[2]=setMat;

	mexCallError= mexCallMATLAB(0, &mexCallOutput, 3, mexCallInputs, "setpref");

}


















