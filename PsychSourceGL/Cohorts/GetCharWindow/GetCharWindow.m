/*
 StoreBitLib.m
 
*/
#include <Carbon/Carbon.h>
#include <Cocoa/Cocoa.h>
#import "KeyRecorderWindow.h"


//local function prototypes
static OSStatus LoadFrameworkBundle(CFStringRef framework, CFBundleRef *bundlePtr);


//file static varibles
static KeyRecorderWindow		*windowObject=NULL;
static NSWindow					*previousKeyWindow=NULL;


void InitializeCocoa(void)
{
	CFBundleRef 				appKitBundleRef;
	OSStatus					err;
	NSApplication				*NSApp;
	
	//	Load the "AppKit.framework" bundl to locate NSApplicationLoad
	//	awi: Simplified Apple's version by removing conditional for pre 10.2.
	err = LoadFrameworkBundle( CFSTR("AppKit.framework"), &appKitBundleRef );
	NSApp=[NSApplication sharedApplication];	
}


void OpenGetCharWindow(void)
{
	NSRect				windowRect;
	unsigned int		windowStyleMask;

	NSAutoreleasePool* pool = [[NSAutoreleasePool alloc] init];

	if(windowObject == NULL){
		windowObject=[KeyRecorderWindow alloc];
		windowRect.origin.x=100;
		windowRect.origin.y=100;
		windowRect.size.width=400;
		windowRect.size.height=400;
		windowStyleMask= NSTitledWindowMask | NSResizableWindowMask;
		windowObject= [windowObject initWithContentRect:windowRect 
									styleMask:windowStyleMask 
									backing:NSBackingStoreBuffered 
									defer:YES];		

		[windowObject orderFront:NULL];
		previousKeyWindow=[NSApp keyWindow];
	}
    [pool release];
}


void CloseGetCharWindow(void)
{
	NSAutoreleasePool* pool = [[NSAutoreleasePool alloc] init];
	
	if(windowObject != NULL){
		[windowObject release];
		windowObject=NULL;
	}

    [pool release];
}


void MakeGetCharWindowVisible(void)
{
	NSAutoreleasePool* pool = [[NSAutoreleasePool alloc] init];
	
	if(windowObject != NULL)
		if([windowObject alphaValue] != (float)1)
			[windowObject setAlphaValue:(float)1];
    [pool release];

}


void MakeGetCharWindowInvisible(void)
{
	NSAutoreleasePool* pool = [[NSAutoreleasePool alloc] init];

	if(windowObject != NULL)
		if([windowObject alphaValue] != (float)0)
			[windowObject setAlphaValue:(float)0];
    [pool release];

}


void StartKeyGathering(void)
{
	NSAutoreleasePool* pool = [[NSAutoreleasePool alloc] init];
	
	[windowObject enableKeypressCollection];

    [pool release];
}


void StopKeyGathering(void)
{
	NSAutoreleasePool* pool = [[NSAutoreleasePool alloc] init];

	[windowObject disableKeypressCollection];

    [pool release];
}


void MakeKeyWindow(void)
{
	NSAutoreleasePool* pool = [[NSAutoreleasePool alloc] init];

	previousKeyWindow=[NSApp keyWindow];
	[windowObject makeKeyWindow];

    [pool release];
}


void RevertKeyWindow(void)
{
	NSAutoreleasePool* pool = [[NSAutoreleasePool alloc] init];

	if([windowObject isKeyWindow] && previousKeyWindow!=NULL){
		[previousKeyWindow makeKeyWindow];
		previousKeyWindow=NULL;
	}
	
	 
    [pool release];
}


CFArrayRef CopyReadKeypressList(void)
{
	NSArray		*keypressList;

	NSAutoreleasePool* pool = [[NSAutoreleasePool alloc] init];

	keypressList=[windowObject readCharList];
	[keypressList retain];
    [pool release];
	return((CFArrayRef)keypressList);

}


CFArrayRef CopyPeekKeypressList(void)
{
	NSArray		*keypressList;

	NSAutoreleasePool* pool = [[NSAutoreleasePool alloc] init];

	keypressList=[windowObject peekCharList];
	[keypressList retain];
    [pool release];
	return((CFArrayRef)keypressList);
}


CFDictionaryRef CopyReadNextKeypress(void)
{
	NSDictionary	*characterDictionary;

	NSAutoreleasePool* pool = [[NSAutoreleasePool alloc] init];
	characterDictionary=[windowObject readNextChar];
	[characterDictionary retain];
    [pool release];
	return((CFDictionaryRef)characterDictionary);

}


CFDictionaryRef CopyPeekNextKeypress(void)
{
	NSDictionary	*characterDictionary;

	NSAutoreleasePool* pool = [[NSAutoreleasePool alloc] init];
	characterDictionary=[windowObject peekNextChar];
	[characterDictionary retain];
    [pool release];
	return((CFDictionaryRef)characterDictionary);
}


void ClearKeypressList(void)
{
	NSAutoreleasePool* pool = [[NSAutoreleasePool alloc] init];

	[windowObject clearCharList]; 
    [pool release];
}


int GetNumKeypresses(void)
{
	int		numChars;
	
	NSAutoreleasePool* pool = [[NSAutoreleasePool alloc] init];
	
	numChars=[windowObject getCharListLength];
    [pool release];
	return(numChars);
}

Boolean	IsKeyWindow(void)
{
	Boolean		returnFlag;
	
	NSAutoreleasePool* pool = [[NSAutoreleasePool alloc] init];
	
	returnFlag= windowObject != NULL && [windowObject isKeyWindow];
    [pool release];
	return(returnFlag);
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

