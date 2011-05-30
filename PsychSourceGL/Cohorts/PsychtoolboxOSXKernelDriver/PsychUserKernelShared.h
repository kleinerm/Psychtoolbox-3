/*
	File:			PsychUserKernelShared.h

	Description:	Definitions shared between PsychtoolboxKernelDriverUserClient (kernel) and PsychtoolboxKernelDriverUserClientTool (userland).

	Copyright:		Copyright © 2008-2011 Mario Kleiner, derived from an Apple example code.

	Change History of original Apple sample code (most recent first):

            1.1			05/22/2007			Perform endian swapping when called from a user process running
											under Rosetta. Updated to produce a universal binary. Now requires
											Xcode 2.2.1 or later to build.
			
			1.0d3	 	01/14/2003			New sample.

*/

// Definitions of GPU registers etc.:
#include "PsychGraphicsCardRegisterSpecs.h"

// Name of the driver to make the connection:
#define kMyDriversIOKitClassName 	"PsychtoolboxKernelDriver"

// Maximum number of input/output arguments:
#define kPsychMaxDriverArgs 20

// Command codes, known by the driver:
enum {
    kPsychKDFastSyncAllHeads = 0,
    kPsychKDGetBeamposition,
	kPsychKDGetStateSnapshot,
	kPsychKDReadRegister,
	KPsychKDWriteRegister,
	kPsychKDDumpMiscInfo,
	kPsychKDSetDitherMode,
    kPsychKDGetRevision,
    kPsychKDGetGPUInfo,
    kPsychKDGetLUTState,
    kPsychKDSetIdentityLUT,
	kPsychKDMaxCommands
};

// Data structure passed between the tool and the user client. 
typedef struct PsychKDCommandStruct {
    UInt32 command;								// Command code: Defines what the driver should do.
    UInt32 inOutArgs[kPsychMaxDriverArgs];		// in/out Arguments: Meaning depends on command code...
} PsychKDCommandStruct;


// Indices into the user client method dispatch table.   
enum {
    kMyUserClientOpen,
    kMyUserClientClose,
    //kMyScalarIStructIMethod,
    //kMyScalarIStructOMethod,
    //kMyScalarIScalarOMethod,
    kPsychKDDispatchCommand,
    kNumberOfMethods // Must be last 
};
