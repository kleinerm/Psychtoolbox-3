/*
	PsychSourceGL/Source/Experiments/StoreBit/StoreBit.c		
  
	PROJECTS: 
  
		StoreBit only
  
	AUTHORS:
  
		Allen.Ingling@nyu.edu		awi 
  
	PLATFORMS:	
  
		Only OS X.
    
	HISTORY:

		9/7/05		awi		Wrote it.
		1/1/27		awi		Changed bundle name back to StoreBitBundle.  It had been changed to getchar while testing that.
		


*/


#include "StoreBit.h"


static void	LoadPrivateFrameworkBundle( CFStringRef framework, CFBundleRef *bundlePtr );

typedef void 	(*InitializeCocoaProc)(void);
typedef	void	(*CocoaStoreBitProc)(Boolean newValue);
typedef Boolean (*CocoaGetBitProc)(void);
typedef void	(*CocoaFreeBitProc)(void);
typedef void	(*CocoaGetWindowCharacterProc)(char *charStringLen2, double *readTime);

CocoaStoreBitProc				CocoaStoreBit=NULL;
CocoaGetBitProc					CocoaGetBit=NULL;
CocoaFreeBitProc				CocoaFreeBit=NULL;
CocoaGetWindowCharacterProc		CocoaGetWindowCharacter=NULL;

static		CFBundleRef			gStoreBitLibBundle;		//	"StoreBitLib.bundle" 


PsychError STOREBITStoreBit(void) 
{
	static Boolean				firstTime=TRUE;
	static Boolean				failedToLoadFlag=FALSE;
    double						returnValue, inputValue;
	static						Boolean cBit=FALSE;
	Boolean						isThere, inputValueBoolean;
	InitializeCocoaProc			CocoaInitializeCocoa;
	char						readChar[2];
	double						readTime;
	const char					*thisMexFunctionName;
//	#define						MAX_MATLAB_CALL_STRING_LENGTH	1024;
//	char						matlabCallString[MAX_MATLAB_CALL_STRING_LENGTH];
	int							matlabCallbackErrorFlag, errorFlag;
	mxArray						*matlabCallbackOutputs, *matlabCallbackInputs[1];
	#define						COCOA_BUNDLE_NAME					"StoreBitLib.bundle"
	#define						SYSTEM_FILE_PATH_SEPARATOR			"/"
	CFStringRef					directoryPathCFString, bundlePathCFString;
	char						*pathToBundleDirectoryStr;
	size_t						pathToBundleDirectoryStrSizeBytes, pathToBundleDirectoryStrSizeChars, bundleNameStrSizeChars, pathToBundleStrSizeChars, charSize, mxCharSize;
	CFStringRef					stringArray[3];
	CFArrayRef					stringCFArray;


	if(failedToLoadFlag){
		//In case we re-enter this mex file and the previous call failed to the load the bundle, then notifiy and exit.
		//The user will have to clear mex to reset, but that's not too high a penalty for a rare and serious problem like a missing library.
		PsychErrorExitMsg(PsychError_internal,"Failed to load the Cocoa Bundle \"StoreBitLib.bundle\".");
	}else{
		if(firstTime){
			firstTime=FALSE;
			//we assume that the bundle is in the same directory as the mex file itself.  
			// We break the PsychScriptingGlue API here by calling a mex API function here directly. Eventually these calls which get the path to this 
			// mex file should be grouped into their function in the scripting glue layer.  We could also make the 0th input argument the function name.
			thisMexFunctionName= mexFunctionName();
			matlabCallbackInputs[0]= mxCreateString(thisMexFunctionName);
			matlabCallbackErrorFlag=mexCallMATLAB(1, &matlabCallbackOutputs, 1, matlabCallbackInputs, "FunctionFolder");
			if(matlabCallbackErrorFlag){
				failedToLoadFlag=TRUE;
				PsychErrorExitMsg(PsychError_internal,"MATLAB callback to \"FunctionFolder.m\" failed.  It could be missing or off the MATLAB path.");
			}
			if(mxGetM(matlabCallbackOutputs) * mxGetN(matlabCallbackOutputs) == 0){
				failedToLoadFlag=TRUE;
				PsychErrorExitMsg(PsychError_internal,"MATLAB callback to \"FunctionFolder.m\" failed.");
			}
			pathToBundleDirectoryStrSizeBytes= (size_t)((mxGetM(matlabCallbackOutputs) * mxGetN(matlabCallbackOutputs) + 1)* sizeof(mxChar));
			pathToBundleDirectoryStr=(char*)malloc(pathToBundleDirectoryStrSizeBytes);
			errorFlag= mxGetString(matlabCallbackOutputs, (char*)pathToBundleDirectoryStr, pathToBundleDirectoryStrSizeBytes);
			if(errorFlag){
				free((void*)pathToBundleDirectoryStr);
				failedToLoadFlag=TRUE;
				PsychErrorExitMsg(PsychError_internal,"Failed to load the Cocoa Bundle because we could not convernt a MATALB string to C string.");
			}
			//now we have in a unicode string the name of the directory in which this mex function, and also presumably its associated bundle reside.  
			//next, get convert it to a CF string and append to that string a path seperator and the name of the bundle.  
			//directoryPathCFString= CFStringCreateWithCharacters(kCFAllocatorDefault, (UniChar*)pathToBundleDirectoryStr, mxGetM(matlabCallbackOutputs) * mxGetN(matlabCallbackOutputs));
//			stringArray[0]= CFStringCreateWithCString(kCFAllocatorDefault, pathToBundleDirectoryStr, kCFStringEncodingUnicode);		// the path to the directory
			stringArray[0]= CFStringCreateWithCString(kCFAllocatorDefault, pathToBundleDirectoryStr, kCFStringEncodingUTF8);		// the path to the directory
			stringArray[1]= CFSTR(SYSTEM_FILE_PATH_SEPARATOR);
			stringArray[2]= CFSTR(COCOA_BUNDLE_NAME);
			stringCFArray= CFArrayCreate(kCFAllocatorDefault, (void*)stringArray, 3, &kCFTypeArrayCallBacks);
			bundlePathCFString= CFStringCreateByCombiningStrings(kCFAllocatorDefault, stringCFArray, CFSTR(""));
			
//			LoadPrivateFrameworkBundle( CFSTR("/StoreBitLib.bundle"), &gStoreBitLibBundle );
//			LoadPrivateFrameworkBundle( CFSTR("/Applications/Psychtoolbox/PsychDemos/StoreBitLib.bundle"), &gStoreBitLibBundle );
			LoadPrivateFrameworkBundle(bundlePathCFString , &gStoreBitLibBundle );
			CFRelease(bundlePathCFString);
			CFRelease(stringArray[0]);
			CFRelease(stringCFArray);
			free((void*)pathToBundleDirectoryStr);
			if ( gStoreBitLibBundle == NULL ){
				failedToLoadFlag=TRUE;
				PsychErrorExitMsg(PsychError_internal,"Failed to load the Cocoa Bundle \"StoreBitLib.bundle\".  It could be missing.");
			}
			
			CocoaInitializeCocoa = (InitializeCocoaProc) CFBundleGetFunctionPointerForName(gStoreBitLibBundle, CFSTR("InitializeCocoa") );
			if(CocoaInitializeCocoa != NULL) 
				CocoaInitializeCocoa();
			CocoaStoreBit= (CocoaStoreBitProc)CFBundleGetFunctionPointerForName(gStoreBitLibBundle, CFSTR("CocoaStoreBit"));
			CocoaGetBit= (CocoaGetBitProc)CFBundleGetFunctionPointerForName(gStoreBitLibBundle, CFSTR("CocoaGetBit"));
			CocoaFreeBit= (CocoaFreeBitProc)CFBundleGetFunctionPointerForName(gStoreBitLibBundle, CFSTR("CocoaFreeBit"));
			CocoaGetWindowCharacter= (CocoaGetWindowCharacterProc)CFBundleGetFunctionPointerForName(gStoreBitLibBundle, CFSTR("CocoaGetWindowCharacter"));
		}//  if(firstTime){
					
		//check to see if the user supplied superfluous arguments
		PsychErrorExit(PsychCapNumOutputArgs(1));
		PsychErrorExit(PsychCapNumInputArgs(1));
		
		//Allocate a return matrix and load it with the depth values.
		returnValue=(double)CocoaGetBit();
		PsychCopyOutDoubleArg(1, kPsychArgOptional, returnValue);
		isThere=PsychCopyInDoubleArg(1, kPsychArgOptional, &inputValue);
		if(isThere){
			inputValueBoolean=(Boolean)inputValue;
			CocoaStoreBit(inputValueBoolean);
		}
//		CocoaGetWindowCharacter(readChar, &readTime);
//		mexPrintf("%s\n", readChar);
//		mexPrintf("%d\n", readTime);
	}
	
    return(PsychError_none);	
}



/*
	LoadPrivateFrameworkBundle
	
	Modified from Apple's version to accept an absolute path in posix style.  Apple's version assumed
	that the private bundled framework resided within the calling application's bundle.  We want this to
	work with MATLAB which is not distributed as an application bundle.
*/ 
static void	LoadPrivateFrameworkBundle( CFStringRef framework, CFBundleRef *bundlePtr )
{
	CFURLRef	baseURL			= NULL;
	CFURLRef	bundleURL		= NULL;
	CFBundleRef	myAppsBundle	= NULL;
	
	if ( bundlePtr == NULL )	goto Bail;
	*bundlePtr = NULL;
	
	myAppsBundle	= CFBundleGetMainBundle();					//	Get our application's main bundle from Core Foundation
	if ( myAppsBundle == NULL )	goto Bail;
	
	baseURL	= CFBundleCopyPrivateFrameworksURL( myAppsBundle );
	if ( baseURL == NULL )	goto Bail;

/*
	bundleURL = CFURLCreateCopyAppendingPathComponent( kCFAllocatorSystemDefault, baseURL, framework, false );
	if ( bundleURL == NULL )	goto Bail;
*/
	//replaces commented out section above.  This function now accepts a posix path to bundle.  
	bundleURL = CFURLCreateWithFileSystemPath (kCFAllocatorSystemDefault, framework, kCFURLPOSIXPathStyle, FALSE);
	if ( bundleURL == NULL )	goto Bail;

	*bundlePtr = CFBundleCreate( kCFAllocatorSystemDefault, bundleURL );
	if ( *bundlePtr == NULL )	goto Bail;

	if ( ! CFBundleLoadExecutable( *bundlePtr ) )
	{
		CFRelease( *bundlePtr );
		*bundlePtr	= NULL;
	}

Bail:															// Clean up.
	if ( bundleURL != NULL )	CFRelease( bundleURL );
	if ( baseURL != NULL )		CFRelease( baseURL );
}



PsychError StoreBitExitFunction(void)
{
	if(CocoaFreeBit != NULL)
		CocoaFreeBit();
		
	if(gStoreBitLibBundle!=NULL)
		CFBundleUnloadExecutable(gStoreBitLibBundle);
}


	
