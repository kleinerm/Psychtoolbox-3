/*
 StoreBitLib.m
 
*/
#include <Carbon/Carbon.h>
#include <Cocoa/Cocoa.h>


//local function prototypes
static OSStatus LoadFrameworkBundle(CFStringRef framework, CFBundleRef *bundlePtr);


//file static variables
static NSNumber					*numberObject=NULL;


/*
	Needed to make sure the Cocoa framework has a chance to initialize things in a Carbon app.

	awi: Simplified Apple's version by removing conditional for pre 10.2.
*/
typedef BOOL (*NSApplicationLoadFuncPtr)( void );

void	InitializeCocoa()
{
	CFBundleRef 				appKitBundleRef;
	OSStatus					err;
	NSApplication				*NSApp;
	
	//	Load the "AppKit.framework" bundl to locate NSApplicationLoad
	err = LoadFrameworkBundle( CFSTR("AppKit.framework"), &appKitBundleRef );
	NSApp=[NSApplication sharedApplication];	
}



void CocoaStoreBit(Boolean newValue)
{

	NSAutoreleasePool* pool = [[NSAutoreleasePool alloc] init];
	 
	if(numberObject != NULL)
		 [numberObject release];
	numberObject=[[NSNumber alloc] initWithBool:newValue];
	
    [pool release];
}


Boolean CocoaGetBit(void)
{
	Boolean				returnValue;
	
	NSAutoreleasePool* pool = [[NSAutoreleasePool alloc] init];
		
	if(numberObject == NULL)
		returnValue=FALSE;
	else
		returnValue=[numberObject boolValue];
			
	[pool release];
	return(returnValue);
}



void CocoaFreeBit(void)
{
	NSAutoreleasePool* pool = [[NSAutoreleasePool alloc] init];

	if(numberObject!=NULL){
		[numberObject release];
		numberObject=NULL;
	}
	
	[pool release];
}


static OSStatus LoadFrameworkBundle(CFStringRef framework, CFBundleRef *bundlePtr)
{
	OSStatus 	err;
	FSRef 		frameworksFolderRef;
	CFURLRef	baseURL;
	CFURLRef	bundleURL;
	
	if ( bundlePtr == nil )	return( -1 );
	
	*bundlePtr = nil;
	
	baseURL = nil;
	bundleURL = nil;
	
	err = FSFindFolder(kOnAppropriateDisk, kFrameworksFolderType, true, &frameworksFolderRef);
	if (err == noErr) {
		baseURL = CFURLCreateFromFSRef(kCFAllocatorSystemDefault, &frameworksFolderRef);
		if (baseURL == nil) {
			err = coreFoundationUnknownErr;
		}
	}
	if (err == noErr) {
		bundleURL = CFURLCreateCopyAppendingPathComponent(kCFAllocatorSystemDefault, baseURL, framework, false);
		if (bundleURL == nil) {
			err = coreFoundationUnknownErr;
		}
	}
	if (err == noErr) {
		*bundlePtr = CFBundleCreate(kCFAllocatorSystemDefault, bundleURL);
		if (*bundlePtr == nil) {
			err = coreFoundationUnknownErr;
		}
	}
	if (err == noErr) {
	    if ( ! CFBundleLoadExecutable( *bundlePtr ) ) {
			err = coreFoundationUnknownErr;
	    }
	}

	// Clean up.
	if (err != noErr && *bundlePtr != nil) {
		CFRelease(*bundlePtr);
		*bundlePtr = nil;
	}
	if (bundleURL != nil) {
		CFRelease(bundleURL);
	}	
	if (baseURL != nil) {
		CFRelease(baseURL);
	}	
	
	return err;
}

