/*
	PsychSourceGL/Source/Experiments/StoreBit/StoreBit.c		
  
	PROJECTS: 
  
		StoreBit only
  
	AUTHORS:
  
		Allen.Ingling@nyu.edu		awi 
  
	PLATFORMS:	
  
		Only OS X.
    
	HISTORY:

		9/7/05		awi		Wrote it .


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
	static Boolean firstTime=TRUE;
    double						returnValue, inputValue;
	static						Boolean cBit=FALSE;
	Boolean						isThere, inputValueBoolean;
	InitializeCocoaProc			CocoaInitializeCocoa;
	char						readChar[2];
	double						readTime;


	if(firstTime){
		firstTime=FALSE;
		LoadPrivateFrameworkBundle( CFSTR("/GetCharWindow.bundle"), &gStoreBitLibBundle );
		if ( gStoreBitLibBundle != NULL )
		{
			CocoaInitializeCocoa	= (InitializeCocoaProc) CFBundleGetFunctionPointerForName(gStoreBitLibBundle, CFSTR("InitializeCocoa") );
			if(CocoaInitializeCocoa != NULL) 
				CocoaInitializeCocoa();
			CocoaStoreBit= (CocoaStoreBitProc)CFBundleGetFunctionPointerForName(gStoreBitLibBundle, CFSTR("CocoaStoreBit"));
			CocoaGetBit= (CocoaGetBitProc)CFBundleGetFunctionPointerForName(gStoreBitLibBundle, CFSTR("CocoaGetBit"));
			CocoaFreeBit= (CocoaFreeBitProc)CFBundleGetFunctionPointerForName(gStoreBitLibBundle, CFSTR("CocoaFreeBit"));
			CocoaGetWindowCharacter= (CocoaGetWindowCharacterProc)CFBundleGetFunctionPointerForName(gStoreBitLibBundle, CFSTR("CocoaGetWindowCharacter"));
		}
	}
		
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
	CocoaGetWindowCharacter(readChar, &readTime);
	mexPrintf("%s\n", readChar);
	mexPrintf("%d\n", readTime);
	
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
}


	