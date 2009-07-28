/*
	File:			PsychtoolboxKernelDriver.h
	
	Description:	This file implements the I/O Kit driver kernel extension for Psychtoolbox (KEXT).

	Copyright:		Copyright © 2008 Mario Kleiner, derived from an Apple example code.

	Change History of original Apple sample code (most recent first):

            1.1			05/22/2007			User client performs endian swapping when called from a user process 
											running using Rosetta. Updated to produce a universal binary.
											Now requires Xcode 2.2.1 or later to build.
			
			1.0d3	 	01/14/2003			New sample.

*/


#include <IOKit/IOService.h>
#include <IOKit/pci/IOPCIDevice.h>
#include <IOKit/IOInterruptEventSource.h>
#include <IOKit/IOFilterInterruptEventSource.h>

// Settings for member fDeviceType:
#define kPsychUnknown 0
#define kPsychGeForce 1
#define kPsychRadeon  2

struct PsychKDCommandStruct;

class PsychtoolboxKernelDriver : public IOService
{
    OSDeclareDefaultStructors(PsychtoolboxKernelDriver)
	
private:
	UInt32							fDeviceType;
    IOPCIDevice *					fPCIDevice;
	IOMemoryMap *					fRadeonMap;
	IOVirtualAddress				fRadeonRegs;
	UInt32							fRadeonSize;
	IOFilterInterruptEventSource*	fInterruptSrc;
	
	UInt32							fInterruptCookie;
	UInt32							fInterruptCounter;
	UInt32							fVBLCounter[2];

	// Initialize our own interrupt handler for snooping on gfx-card state changes:
	bool InitializeInterruptHandler(void);	

	// Fast-Path interrupt handler: Gets called in direct primary hardware interrupt context!
	bool fastPathInterruptHandler(OSObject* myself, IOFilterInterruptEventSource* mySource);

	// Slow-Path WorkLoop interrupt handler: Gets called if the fast-path handler returns "true".
	void workLoopInterruptHandler(OSObject* myself, IOInterruptEventSource* mySource, int pendingIRQs);
	
	// Handle VBLANK IRQ's for display head 'headId':
	void handleVBLIRQ(UInt32 headId);
	
	// Read 32 bit control register at 'offset':
	UInt32	ReadRegister(UInt32 offset);

	// Write 32 bit control register at 'offset' with 'value':
	void	WriteRegister(UInt32 offset, UInt32 value);

	// Helper function for SetDitherMode() on G80 GPUs:
	void    G80DispCommand(UInt32 addr, UInt32 data);
	
	// Return current vertical rasterbeam position of display head 'headId' (0=Primary CRTC1, 1=Secondary CRTC2):
	UInt32 GetBeamPosition(UInt32 headId);
	
	// Instantaneously resynchronize display heads of a Radeon dual-head gfx-card:
	SInt32	FastSynchronizeAllDisplayHeads(void);
	
	// Try to change hardware dither mode on GPU:
	void SetDitherMode(UInt32 headId, UInt32 ditherOn);
	
	// Perform instant state snapshot of interesting registers and return'em:
	void	GetStateSnapshot(PsychKDCommandStruct* outStruct);
	
	// Dump interesting register state to system log:
	void	DumpGfxState(void);
	
public:
	// IOService methods
	virtual bool init(OSDictionary* dictionary = 0);
	virtual void free(void);
	
	virtual IOService* probe(IOService* provider, SInt32* score);
	
	virtual bool start(IOService* provider);
	virtual void stop(IOService* provider);
	
	virtual bool willTerminate(IOService* provider, IOOptionBits options);
	virtual bool didTerminate(IOService* provider, IOOptionBits options, bool* defer);
	
    virtual bool terminate(IOOptionBits options = 0);
    virtual bool finalize(IOOptionBits options);

	// SimpleDriver methods
//	virtual IOReturn ScalarIStructI(int inNumber, MySampleStruct* inStruct, IOByteCount inStructSize);
//	virtual IOReturn ScalarIStructO(int inNumber1, int inNumber2, MySampleStruct* outStruct, IOByteCount* outStructSize);
//	virtual IOReturn ScalarIScalarO(int inNumber1, int inNumber2, int* outNumber);
	virtual IOReturn PsychKDDispatchCommand(PsychKDCommandStruct* inStruct, PsychKDCommandStruct* outStruct, IOByteCount inStructSize, IOByteCount* outStructSize);
};