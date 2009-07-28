/*
	File:			PsychUserKernelShared.h

	Description:	Definitions shared between PsychtoolboxKernelDriverUserClient (kernel) and PsychtoolboxKernelDriverUserClientTool (userland).

	Copyright:		Copyright © 2008 Mario Kleiner, derived from an Apple example code.

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

// The following register offsets and specs are taken from the official AMD/ATI
// specs, downloadable from http://www.x.org/docs/AMD/ the website of the X-ORG
// foundation, home of the open source Linux X11 server. These specs are official
// specs from AMD/ATI, released in autumn 2007 to the open-source community.

// The D1CRTC_STATUS_POSITION register (32 bits) encodes vertical beam position in
// bits 0:12 (the least significant 13 bits), and horizontal beam position in
// bits 16-28. D2 is the secondary display pipeline (e.g., the external video port
// on a laptop), whereas D1 is the primary pipeline (e.g., internal panel of a laptop).
// The addresses and encoding is the same for the X1000 series and the newer HD2000/3000
// series chips...
#define RADEON_D1CRTC_STATUS_POSITION 0x60a0
#define RADEON_D2CRTC_STATUS_POSITION 0x68a0
#define RADEON_VBEAMPOSITION_BITMASK  0x1fff
#define RADEON_HBEAMPOSITION_BITSHIFT 16

// This (if we would use it) would give access to on-chip frame counters. These increment
// once per video refresh cycle - at the beginning of a new cycle (scanline zero) and
// can be software reset, but normally start at system bootup with zero. Not yet sure
// if we should/would wanna use 'em but we'll see...
#define RADEON_D1CRTC_STATUS_FRAME_COUNT 0x60a4
#define RADEON_D2CRTC_STATUS_FRAME_COUNT 0x68a4

// CRTC display scan converter master enable register: Bit 0 enables/disables CRTC1 head,
// whereas Bit 1 enables/disables CRTC2 head. Writing a zero into the reg. will disable and
// blank both displays, Writing a 0x1 | 0x2 == 0x3 will simulataneously enable both heads.
// --> As writing one register is atomic and both heads reset their beamposition on restart,
// this is a simple way to instantaneously and perfectly sync the two display heads.
#define RADEON_DC_CRTC_MASTER_ENABLE 0x60f8

// Registers which point to the start addresses of the onscreen framebuffers - the
// "OpenGL frontbuffers".
// In mono mode, primary surface is the frontbuffer memory pointer, in frame-sequential
// stereo mode, primary surface is the front-left buffer, secondary surface is the front
// right buffer. They don't change in OS/X windowed mode, ie., when the normal user interface
// is active, as the Quartz compositor seems to use fast VRAM->VRAM blits to recomposit
// the user interface, not double-buffering via page-flipping. In fullscreen mode
// (fullscreen onscreen windows open), they change at each Screen('Flip') execution, ie.
// at each completed double-buffer swap. They toggle between the VRAM addresses of the two
// (or four in stereo) buffers when they are exchanged:
#define RADEON_D1GRPH_PRIMARY_SURFACE_ADDRESS	0x6110 
#define RADEON_D1GRPH_SECONDARY_SURFACE_ADDRESS 0x6118
#define RADEON_D2GRPH_PRIMARY_SURFACE_ADDRESS	0x6910 
#define RADEON_D2GRPH_SECONDARY_SURFACE_ADDRESS 0x6918

// Bit 0 of these enables (Setting of 0) or disables (Setting of 1 aka 0x1) internal routing
// the VSYNC signal (aka V_UPDATE), so this can prevent/hold back the double-buffered
// update of CRTC registers, esp. upate of the primary/secondary surface address.
// TODO TEST: Can this be used to delay flips, e.g., until some deadline is reached
// or some user defined trigger condition occurs, e.g., kernel-level sync swaps to
// trigger condition?
#define RADEON_D1MODE_MASTER_UPDATE_LOCK		0x60E0
#define RADEON_D2MODE_MASTER_UPDATE_LOCK		0x68E0

// Encodes pitch (length of a single scanline in pixels) as stored in the framebuffer:
#define RADEON_D1GRPH_PITCH 0x6120
#define RADEON_D2GRPH_PITCH 0x6920 

// These registers control operation of the color/gamma hardware lookup tables:
// Bit 0 selects if hardware LUT A or B should be used for the given display pipeline.
// Bit 8 controls if the LUT's should be bypassed in ABGR2101010 operation mode, ie.,
// if a 10 bit per color channel framebuffer is used, should the 8->10 LUT's be ignored
// and the 10 bit values directly output to the RAMDAC's/TDMS/LVDS emitters? Usually
// you want to enable this bypass in 10 bpc mode for best results:
#define RADEON_D1GRPH_LUT_SEL	0x6108
#define RADEON_D2GRPH_LUT_SEL	0x6908

// These registers control the color depth and pixel formatting in the framebuffer -
// or better: The interpretation of the data by the CRTC's. They don't affect the
// rasterizer backends - the GPU will always draw ABGR8888 formatted data in 32 bit
// color mode. Bits 8-10 control data format. Especially interesting: If you choose
// 32 bit color depths (Bits 1:0 == 0x2) and you set Bits 10:8 to 0x1,ie. set bit 8,
// then the 32 bit pixel data will be interpreted as ABGR2101010 data, ie. 2 bits
// alpha, 10 bits for red, green and blue. Enabling LUT bypass (see above) will give
// you a real linear 10 bpc framebuffer from input to output! However, as the GPU
// always renders in 8888 format, you'll experience corrupted screen colors, unless
// you use the PTB imaging pipeline for automatic reformatting of floating point
// HDR images to RGBA8 formatted data, suitable for 2101010 mode. :-)
#define RADEON_D1GRPH_CONTROL	0x6104
#define RADEON_D2GRPH_CONTROL	0x6904

// These registers control output bit depths over the TDMS (DVI-Digital) and
// LVDS (internal flat panel drivers) emitters, as well as the method to reduce
// the bit depths in a visually pleasing way if reduction is enabled.
// Writing all zero (0x0) will disable reduction, ie., output a 24bpp, 8bpc
// stream. Available methods of reduction are:
// a) Truncation: Cutting off least significant bits (Enable by setting bit 0),
// b) Spatial dithering: Set bit 8.
// c) Temporal dithering/modulation: Set bit 16.
// Bits 4 for a), 12 for b) and 20 for c) Control bpp: either 24 (bits cleared) or
// 18 (bits sets)
// There are more higher order bits for controlling exact operation of temporal
// dithering...
// TODO CHECK: If there is only one control for TDMS, does this mean that the
// same method of dithering/reduction always applies to both DVI ports on a
// dual-head card? That would probably mean that the achievable bit depths
// of a DVI connected device is always the one of the least capable of two
// connected displays (e.g., A) Bits++, B) Cheap 18bpp panel --> Output 18 bpp
// screwup for Bits++)???
#define RADEON_LVTMA_BIT_DEPTH_CONTROL	0x7A94
#define RADEON_TMDSA_BIT_DEPTH_CONTROL	0x7894
