/*
 StoreBitLib.h
*/

#include <Carbon/Carbon.h>
#include <Cocoa/Cocoa.h>


void	InitializeCocoa();
void	CocoaStoreBitProc(Boolean newValue);
Boolean CocoaGetBitProc(void);
void	CocoaFreeBitProc(void);
