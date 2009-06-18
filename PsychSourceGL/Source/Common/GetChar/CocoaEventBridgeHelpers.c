/*
	PsychSourceGL/Source/Common/CocoaEventBridge/CocoaEventBridgeHelpers.c		
  
	PROJECTS: 
  
		CocoaEventBridge only
  
	AUTHORS:
  
		Allen.Ingling@nyu.edu		awi 
  
	PLATFORMS:	
  
		Only OS X.
    
	HISTORY:

		9/14/05		awi		Wrote it .


*/


#include "CocoaEventBridgeHelpers.h"

void GetPathToBundleString(char **fPath);


InitializeCocoaProc				InitializeCocoa = NULL;
OpenGetCharWindowProc			OpenGetCharWindow= NULL;
CloseGetCharWindowProc			CloseGetCharWindow = NULL;
MakeGetCharWindowVisibleProc			MakeGetCharWindowVisible = NULL;
MakeGetCharWindowInvisibleProc	MakeGetCharWindowInvisible = NULL;
StartKeyGatheringProc			StartKeyGathering = NULL;
StopKeyGatheringProc			StopKeyGathering = NULL;
MakeKeyWindowProc				MakeKeyWindow = NULL;
RevertKeyWindowProc				RevertKeyWindow = NULL;
CopyReadKeypressListProc		CopyReadKeypressList = NULL;
CopyPeekKeypressListProc		CopyPeekKeypressList = NULL;
CopyReadNextKeypressProc		CopyReadNextKeypress = NULL;
CopyPeekNextKeypressProc		CopyPeekNextKeypress = NULL;
ClearKeypressListProc			ClearKeypressList = NULL;
GetNumKeypressesProc			GetNumKeypresses = NULL;
IsKeyWindowProc					IsKeyWindow=NULL;

static void	LoadPrivateFrameworkBundle( CFStringRef framework, CFBundleRef *bundlePtr );
static	CFBundleRef	gStoreBitLibBundle=NULL;		//	"StoreBitLib.bundle" 

psych_bool LoadCocoaBundle(void) 
{
	static psych_bool				firstTime=TRUE;
	psych_bool						failed;
	static	psych_bool				foundAllFunctions;
	char						*localPathToCocoaBundleStr;
	CFStringRef					localPathToCocoaBundle;

	if(firstTime){
		firstTime=FALSE;
		GetPathToBundleString(&localPathToCocoaBundleStr);
		if(!strcmp(localPathToCocoaBundleStr, "")) {
                        mexPrintf("CEB: GetPathToBundleString() failed!");
			return(TRUE);
                }
		localPathToCocoaBundle= CFStringCreateWithBytes(kCFAllocatorDefault, 
														localPathToCocoaBundleStr, 
														strlen(localPathToCocoaBundleStr), 
														kCFStringEncodingUTF8,
														FALSE);
		LoadPrivateFrameworkBundle( localPathToCocoaBundle, &gStoreBitLibBundle );
		CFRelease(localPathToCocoaBundle);
//		LoadPrivateFrameworkBundle( CFSTR("/GetCharWindow.bundle"), &gStoreBitLibBundle );
		if ( gStoreBitLibBundle != NULL ) {
			InitializeCocoa	= (InitializeCocoaProc) CFBundleGetFunctionPointerForName(gStoreBitLibBundle, CFSTR("InitializeCocoa") );
                    if(InitializeCocoa != NULL)
				InitializeCocoa();
                    else {
                        mexPrintf("CEB: CFBundleGetFunctionPointerForName('InitializeCocoa') failed!");
                        return(TRUE);
                    }
			OpenGetCharWindow= (OpenGetCharWindowProc)CFBundleGetFunctionPointerForName(gStoreBitLibBundle, CFSTR("OpenGetCharWindow"));
			CloseGetCharWindow= (CloseGetCharWindowProc)CFBundleGetFunctionPointerForName(gStoreBitLibBundle, CFSTR("CloseGetCharWindow"));
			MakeGetCharWindowVisible= (MakeGetCharWindowVisibleProc)CFBundleGetFunctionPointerForName(gStoreBitLibBundle, CFSTR("MakeGetCharWindowVisible"));
			MakeGetCharWindowInvisible= (MakeGetCharWindowInvisibleProc)CFBundleGetFunctionPointerForName(gStoreBitLibBundle, CFSTR("MakeGetCharWindowInvisible"));
			StartKeyGathering= (StartKeyGatheringProc)CFBundleGetFunctionPointerForName(gStoreBitLibBundle, CFSTR("StartKeyGathering"));
			StopKeyGathering= (StopKeyGatheringProc)CFBundleGetFunctionPointerForName(gStoreBitLibBundle, CFSTR("StopKeyGathering"));
			MakeKeyWindow= (MakeKeyWindowProc)CFBundleGetFunctionPointerForName(gStoreBitLibBundle, CFSTR("MakeKeyWindow"));
			RevertKeyWindow= (RevertKeyWindowProc)CFBundleGetFunctionPointerForName(gStoreBitLibBundle, CFSTR("RevertKeyWindow"));
			CopyReadKeypressList= (CopyReadKeypressListProc)CFBundleGetFunctionPointerForName(gStoreBitLibBundle, CFSTR("CopyReadKeypressList"));
			CopyPeekKeypressList= (CopyPeekKeypressListProc)CFBundleGetFunctionPointerForName(gStoreBitLibBundle, CFSTR("CopyPeekKeypressList"));
			CopyReadNextKeypress= (CopyReadNextKeypressProc)CFBundleGetFunctionPointerForName(gStoreBitLibBundle, CFSTR("CopyReadNextKeypress"));
			CopyPeekNextKeypress= (CopyPeekNextKeypressProc)CFBundleGetFunctionPointerForName(gStoreBitLibBundle, CFSTR("CopyPeekNextKeypress"));
			ClearKeypressList= (ClearKeypressListProc)CFBundleGetFunctionPointerForName(gStoreBitLibBundle, CFSTR("ClearKeypressList"));
			GetNumKeypresses= (GetNumKeypressesProc)CFBundleGetFunctionPointerForName(gStoreBitLibBundle, CFSTR("GetNumKeypresses"));
			IsKeyWindow= (IsKeyWindowProc)CFBundleGetFunctionPointerForName(gStoreBitLibBundle, CFSTR("IsKeyWindow"));
		}else {
                    mexPrintf("CEB: LoadPrivateFrameworkBundle(%s) failed!", localPathToCocoaBundleStr);
                    return(TRUE);
                }
                
		foundAllFunctions= OpenGetCharWindow && CloseGetCharWindow && MakeGetCharWindowVisible && MakeGetCharWindowInvisible && StartKeyGathering && StopKeyGathering 
							&& MakeKeyWindow && CopyReadKeypressList && CopyPeekKeypressList && CopyReadNextKeypress 
							&& CopyPeekNextKeypress && ClearKeypressList && GetNumKeypresses && IsKeyWindow && RevertKeyWindow;
                if (!foundAllFunctions) mexPrintf("CEB: Could not retrieve function pointers to all bundle-functions!");
	}
	return(!foundAllFunctions);
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




	
