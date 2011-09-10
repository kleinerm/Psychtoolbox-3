/*
	PsychToolbox3/Source/Linux/Screen/PsychScreenGlue.c
	
	PLATFORMS:	
	
		This is the Linux/X11 version only.  
				
	AUTHORS:
	
	Mario Kleiner		mk		mario.kleiner at tuebingen.mpg.de

	HISTORY:
	
	2/20/06                 mk              Wrote it. Derived from Windows version.
        							
	DESCRIPTION:
	
		Functions in this file comprise an abstraction layer for probing and controlling screen state.  
		
		Each C function which implements a particular Screen subcommand should be platform neutral.  For example, the source to SCREENPixelSizes() 
		should be platform-neutral, despite that the calls in OS X and Linux to detect available pixel sizes are
		different.  The platform specificity is abstracted out in C files which end it "Glue", for example PsychScreenGlue, PsychWindowGlue, 
		PsychWindowTextClue.
	
		In addition to glue functions for windows and screen there are functions which implement shared functionality between between Screen commands,
		such as ScreenTypes.c and WindowBank.c. 
			
	NOTES:
	
	TO DO: 

*/


#include "Screen.h"


/* These are needed for our GPU specific beamposition query implementation: */
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/mman.h>

// To us libpciaccess for GPU device detection and mmaping():
#include "pciaccess.h"
#define PCI_CLASS_DISPLAY	0x03

// We build with VidModeExtension support unless forcefully disabled at compile time via a -DNO_VIDMODEEXTS
#ifndef NO_VIDMODEEXTS
#define USE_VIDMODEEXTS 1
#endif

#ifdef USE_VIDMODEEXTS
// Functions for setup and query of hw gamma CLUTS and for monitor refresh rate query:
#include <X11/extensions/xf86vmode.h>

#else
#define XF86VidModeNumberErrors 0
#endif


// Event and error base for XRandR extension:
int xr_event, xr_error;
psych_bool has_xrandr_1_2 = FALSE;
psych_bool has_xrandr_1_3 = FALSE;

/* Following structures are needed by our ATI/AMD/NVIDIA beamposition query implementation: */
/* Location and format of the relevant hardware registers of the ATI R500/R600 chips
 * was taken from the official register spec for that chips which was released to
 * the public by AMD/ATI end of 2007 and is available for download at XOrg.
 *
 * http://www.x.org/docs/AMD/
 *
 * Register spec's for DCE-4 hardware are from Linux kms driver and Alex Deucher.
 * This should work on any AVIVO or DCE4/5 display hardware chip, i.e., R300 and
 * later. It won't work on ancient pre-AVIVO hardware.
 */

#include "PsychGraphicsCardRegisterSpecs.h"
#include <endian.h>

// gfx_cntl_mem is mapped to the actual device's memory mapped control area.
// Not the address but what it points to is volatile.
struct pci_device *gpu = NULL;
unsigned char * volatile gfx_cntl_mem = NULL;
unsigned long gfx_length = 0;
unsigned long gfx_lowlimit = 0;
unsigned int  fDeviceType = 0;
unsigned int  fCardType = 0;
unsigned int  fPCIDeviceId = 0;
unsigned int  fNumDisplayHeads = 0;

// Count of kernel drivers:
static int    numKernelDrivers = 0;

// Offset of crtc blocks of evergreen gpu's for each of the six possible crtc's:
unsigned int crtcoff[(DCE4_MAXHEADID + 1)] = { EVERGREEN_CRTC0_REGISTER_OFFSET, EVERGREEN_CRTC1_REGISTER_OFFSET, EVERGREEN_CRTC2_REGISTER_OFFSET, EVERGREEN_CRTC3_REGISTER_OFFSET, EVERGREEN_CRTC4_REGISTER_OFFSET, EVERGREEN_CRTC5_REGISTER_OFFSET };
// unsigned int crtcoff[(DCE4_MAXHEADID + 1)] = { 0x6df0, 0x79f0, 0x105f0, 0x111f0, 0x11df0, 0x129f0 };

/* Is a given ATI/AMD GPU a DCE5 type ASIC, i.e., with the new display engine? */
static psych_bool isDCE5(int screenId)
{
	psych_bool isDCE5 = false;

	// Everything after BARTS is DCE5 -- This is the "Northern Islands" GPU family.
	// Barts, Turks, Caicos, Cayman, Antilles in 0x67xx range:
	if ((fPCIDeviceId & 0xFF00) == 0x6700) isDCE5 = true;

	return(isDCE5);
}

/* Is a given ATI/AMD GPU a DCE4 type ASIC, i.e., with the new display engine? */
static psych_bool isDCE4(int screenId)
{
	psych_bool isDCE4 = false;

	// Everything after CEDAR is DCE4. The Linux radeon kms driver defines
	// in radeon_family.h which chips are CEDAR or later, and the mapping to
	// these chip codes is done by matching against pci device id's in a
	// mapping table inside linux/include/drm/drm_pciids.h
	// Maintaining a copy of that table is impractical for PTB, so we simply
	// check which range of PCI device id's is covered by the DCE-4 chips and
	// code up matching rules here. This should do for now...
	
	// Caiman, Cedar, Redwood, Juniper, Cypress, Hemlock in 0x6xxx range:
	if ((fPCIDeviceId & 0xF000) == 0x6000) isDCE4 = true;
	
	// Palm in 0x98xx range:
	if ((fPCIDeviceId & 0xFF00) == 0x9800) isDCE4 = true;

	return(isDCE4);
}

// Helper routine: Read a single 32 bit unsigned int hardware register at
// offset 'offset' and return its value:
static unsigned int ReadRegister(unsigned long offset)
{
	unsigned int value;

	// Safety check: Don't allow reads past devices MMIO range:
	// We don't return error codes and don't log the problem,
	// because we could be called from primary Interrupt path, so IOLog() is not
	// an option!
	if (gfx_cntl_mem == NULL || offset > gfx_length-4 || offset < gfx_lowlimit) return(0);
	
	// Read and return value:
	value = *(unsigned int * volatile)(gfx_cntl_mem + offset);

	// Enforce a full memory barrier: This is a gcc intrinsic:
	__sync_synchronize();  

	// Radeon: Don't know endianity behaviour: Play save, stick to LE assumption for now:
	if (fDeviceType == kPsychRadeon) return(le32toh(value));

	// Read the register in native byte order: At least NVidia GPU's adapt their
	// endianity to match the host systems endianity, so no need for conversion:
	if (fDeviceType == kPsychGeForce)  return(value);
	if (fDeviceType == kPsychIntelIGP) return(value);

	// No-Op return:
	printf("PTB-ERROR: In GPU ReadRegister(): UNKNOWN fDeviceType of GPU! NO OPERATION!\n");
	return(0);
}

// Helper routine: Write a single 32 bit unsigned int hardware register at
// offset 'offset':
static void WriteRegister(unsigned long offset, unsigned int value)
{
	// Safety check: Don't allow reads past devices MMIO range:
	// We don't return error codes and don't log the problem,
	// because we could be called from primary Interrupt path, so IOLog() is not
	// an option!
	if (gfx_cntl_mem == NULL || offset > gfx_length-4 || offset < gfx_lowlimit) return;

	// Write the register in native byte order: At least NVidia GPU's adapt their
	// endianity to match the host systems endianity, so no need for conversion:
	if (fDeviceType == kPsychGeForce)  value = value;
	if (fDeviceType == kPsychIntelIGP) value = value;

	// Radeon: Don't know endianity behaviour: Play save, stick to LE assumption for now:
	if (fDeviceType == kPsychRadeon) value = htole32(value);

	*(unsigned int* volatile)(gfx_cntl_mem + offset) = value;
	
	// Enforce a full memory barrier: This is a gcc intrinsic:
	__sync_synchronize();  
}

void PsychScreenUnmapDeviceMemory(void)
{
	// Any mapped?
	if (gfx_cntl_mem) {
		// Unmap:
		pci_device_unmap_range(gpu, (void*) gfx_cntl_mem, gfx_length);
		gfx_cntl_mem = NULL;
		gfx_length = 0;
		gpu = NULL;
	}

	// Shutdown PCI access library, release all resources:
	pci_system_cleanup();

	return;
}

// Helper routine: Check if a supported GPU is installed, and mmap() its MMIO register
// control block into our address space for direct register access:
psych_bool PsychScreenMapRadeonCntlMemory(void)
{
	struct pci_device_iterator *iter;
	struct pci_device *dev;
	struct pci_mem_region *region;
	int ret;
	int screenId = 0;
	int currentgpuidx = 0, targetgpuidx = -1;

	// A bit of a hack for now: Allow user to select which gpu in a multi-gpu
	// system should be used for low-level mmio based features. If the environment
	// variable PSYCH_USE_GPUIDX is set to a number, it will try to use that GPU:
	// TODO: Replace this by true multi-gpu support and - far in the future? -
	// automatic mapping of screens to gpu's:
	if (getenv("PSYCH_USE_GPUIDX")) {
		targetgpuidx = atoi(getenv("PSYCH_USE_GPUIDX"));
		if (PsychPrefStateGet_Verbosity() > 2) printf("PTB-INFO: Will try to use GPU number %i for low-level access during this session.\n", targetgpuidx);
	}

	// Safe-guard:
	if (gfx_cntl_mem || gpu) {
		if (PsychPrefStateGet_Verbosity() > 1) printf("PTB-WARNING: Redundant call to PsychScreenMapRadeonCntlMemory()! Ignored for now. This should not happen!\n");
		return(TRUE);
	}

	// Start with default setting: No low-level access possible.
	gfx_cntl_mem = NULL;
	gfx_length = 0;
	gpu = NULL;

	// Initialize libpciaccess:
	ret = pci_system_init();
	if (ret) {
		if (PsychPrefStateGet_Verbosity() > 1) printf("PTB-WARNING: Could not establish low-level access to GPU for screenId %i - Could not initialize PCI system.\n", screenId);
		return(FALSE);
	}

	// Enumerate them:
	iter = pci_id_match_iterator_create(NULL);
	while ((dev = pci_device_next(iter)) != NULL) {
		if (PsychPrefStateGet_Verbosity() > 4) {
			printf("PTB-DEBUG: Checking PCI device [%s %s] with class x%08x ...\n", pci_device_get_vendor_name(dev), pci_device_get_device_name(dev), dev->device_class);
		}

		// "Upgrade" pre PCI 2.0 class device to PCI 2.0 class equivalent
		// to simplify matching:
		if (dev->device_class == 0x00000101) dev->device_class = 0x00030000;
		
		// GPU aka display device class?
		if ((dev->device_class & 0x00ff0000) == (PCI_CLASS_DISPLAY << 16)) {
			// dev is our current candidate gpu. Matching vendor?
			if (dev->vendor_id == PCI_VENDOR_ID_NVIDIA || dev->vendor_id == PCI_VENDOR_ID_ATI || dev->vendor_id == PCI_VENDOR_ID_AMD || dev->vendor_id == PCI_VENDOR_ID_INTEL) {
				// Yes. This is our baby from NVidia or ATI/AMD or Intel:

				// Select the targetgpuidx'th detected gpu:
				// TODO: Replace this hack by true multi-gpu support and - far in the future? -
				// automatic mapping of screens to gpu's:
				if (currentgpuidx >= targetgpuidx) {
					if ((PsychPrefStateGet_Verbosity() > 2) && (targetgpuidx >= 0)) printf("PTB-INFO: Choosing GPU number %i for low-level access during this session.\n", currentgpuidx);

					// Assign as gpu:
					gpu = dev;
					break;
				}

				currentgpuidx++;
			}
		}
	}

	// Enumeration finished - Release iterator:
	pci_iterator_destroy(iter);

	// Found matching GPU?
	if (gpu) {
		// Yes!
		if (PsychPrefStateGet_Verbosity() > 2) {
			printf("PTB-INFO: %s - %s GPU found. Trying to establish low-level access...\n", pci_device_get_vendor_name(gpu), pci_device_get_device_name(gpu));
			fflush(NULL);
		}
		
		// Pull in remaining info about gpu:
		ret = pci_device_probe(gpu);
		if (ret) {
			if (PsychPrefStateGet_Verbosity() > 1) {
				printf("PTB-INFO: Could not probe properties of GPU for screenId %i [%s]\n", screenId, strerror(ret));
				printf("PTB-INFO: Beamposition timestamping and other special features disabled.\n");
				fflush(NULL);
			}

			gpu = NULL;

			// Cleanup:
			pci_system_cleanup();
			
			return(FALSE);
		}

		// Store PCI device id:
		fPCIDeviceId = gpu->device_id;
		
		// Find out which BAR to use for mapping MMIO registers. Depends on GPU vendor:
		if (gpu->vendor_id == PCI_VENDOR_ID_NVIDIA) {
			// BAR 0 is MMIO:
			region = &gpu->regions[0];
			fDeviceType = kPsychGeForce;
			fNumDisplayHeads = 2;
		}

		if (gpu->vendor_id == PCI_VENDOR_ID_ATI || gpu->vendor_id == PCI_VENDOR_ID_AMD) {
			// BAR 2 is MMIO:
			region = &gpu->regions[2];
			fDeviceType = kPsychRadeon;
			fNumDisplayHeads = 2;
		}
		
		if (gpu->vendor_id == PCI_VENDOR_ID_INTEL) {
			// On non GEN-2 hardware, BAR 0 is MMIO:
			region = &gpu->regions[0];
			fCardType = 0;

			// On GEN-2 hardware, BAR 1 is MMIO: Detect known IGP's of GEN-2.
			if ((fPCIDeviceId == 0x3577) || (fPCIDeviceId == 0x2562) || (fPCIDeviceId == 0x3582) || (fPCIDeviceId == 0x358e) || (fPCIDeviceId == 0x2572)) {
				region = &gpu->regions[1];
				fCardType = 2;
			}

			fDeviceType = kPsychIntelIGP;
			fNumDisplayHeads = 2;
		}

		// Try to MMAP MMIO registers with write access, assign their base address to gfx_cntl_mem on success:
		if (PsychPrefStateGet_Verbosity() > 4) {
			printf("PTB-DEBUG: Mapping GPU BAR address %p ...\n", region->base_addr);
			printf("PTB-DEBUG: Mapping %p bytes...\n", region->size);
			fflush(NULL);
		}

		ret = pci_device_map_range(gpu, region->base_addr, region->size, PCI_DEV_MAP_FLAG_WRITABLE, (void**) &gfx_cntl_mem);		
		if (ret || (NULL == gfx_cntl_mem)) {
			if (PsychPrefStateGet_Verbosity() > 1) {
				printf("PTB-INFO: Failed to map GPU low-level control registers for screenId %i [%s].\n", screenId, strerror(ret));
				printf("PTB-INFO: Beamposition timestamping and other special functions disabled.\n");
				printf("PTB-INFO: You must run Matlab/Octave with root privileges for this to work.\n");
				printf("PTB-INFO: However, if you are using the free graphics drivers, there isn't any need for this.\n");
				fflush(NULL);
			}
			
			// Failed:
			gpu = NULL;

			// Cleanup:
			pci_system_cleanup();

			return(FALSE);
		}
		
		// Success! Identify GPU:
		gfx_length = region->size;

		// Lowest allowable MMIO register offset for given GPU:
		gfx_lowlimit = 0;
		
		if (fDeviceType == kPsychGeForce) {
			fCardType = PsychGetNVidiaGPUType(NULL);
			if (PsychPrefStateGet_Verbosity() > 2) {
				printf("PTB-INFO: Connected to NVidia %s GPU of NV-%02x family. Beamposition timestamping enabled.\n", pci_device_get_device_name(gpu), fCardType);
				fflush(NULL);
			}
		}
		
		if (fDeviceType == kPsychRadeon) {
			// On Radeons we distinguish between Avivo (10) or DCE-4 style (40) or DCE-5 (50) for now.
			fCardType = isDCE5(screenId) ? 50 : (isDCE4(screenId) ? 40 : 10);
            
			// On DCE-4 and later GPU's (Evergreen) we limit the minimum MMIO
			// offset to the base address of the 1st CRTC register block for now:
			if (isDCE4(screenId) || isDCE5(screenId)) {
				gfx_lowlimit = 0x6df0;
                
				// Also, DCE-4 supports up to six display heads:
				fNumDisplayHeads = 6;
			}
			
			if (PsychPrefStateGet_Verbosity() > 2) {
				printf("PTB-INFO: Connected to %s %s GPU with %s display engine. Beamposition timestamping enabled.\n", pci_device_get_vendor_name(gpu), pci_device_get_device_name(gpu), (fCardType >= 40) ? ((fCardType >= 50) ? "DCE-5" : "DCE-4") : "AVIVO");
				fflush(NULL);
			}
		}
		
		if (fDeviceType == kPsychIntelIGP) {
			if (PsychPrefStateGet_Verbosity() > 2) {
				printf("PTB-INFO: Connected to Intel %s GPU%s. Beamposition timestamping enabled.\n", pci_device_get_device_name(gpu), (fCardType == 2) ? " of GEN-2 type" : "");
				fflush(NULL);
			}
		}

		// Perform auto-detection of screen to head mappings:
		PsychAutoDetectScreenToHeadMappings(fNumDisplayHeads);

		// Ready to rock!
	} else {
		// No candidate.
		if (PsychPrefStateGet_Verbosity() > 2) printf("PTB-INFO: No suitable low-level controllable GPU found for screenId %i. Beamposition timestamping and other special functions disabled.\n", screenId);
		fflush(NULL);
		
		// Cleanup:
		pci_system_cleanup();
	}
	
	// Return final success or failure status:
	return((gfx_cntl_mem) ? TRUE : FALSE);
}

// Maybe use NULLs in the settings arrays to mark entries invalid instead of using psych_bool flags in a different array.   
static psych_bool		displayLockSettingsFlags[kPsychMaxPossibleDisplays];
static CFDictionaryRef	        displayOriginalCGSettings[kPsychMaxPossibleDisplays];        	//these track the original video state before the Psychtoolbox changed it.  
static psych_bool		displayOriginalCGSettingsValid[kPsychMaxPossibleDisplays];
static CFDictionaryRef	        displayOverlayedCGSettings[kPsychMaxPossibleDisplays];        	//these track settings overlayed with 'Resolutions'.  
static psych_bool		displayOverlayedCGSettingsValid[kPsychMaxPossibleDisplays];
static psych_bool               displayBeampositionHealthy[kPsychMaxPossibleDisplays];
static CGDisplayCount 		numDisplays;

// displayCGIDs stores the X11 Display* handles to the display connections of each PTB logical screen:
static CGDirectDisplayID 	displayCGIDs[kPsychMaxPossibleDisplays];
// displayX11Screens stores the mapping of PTB screenNumber's to corresponding X11 screen numbers:
static int                      displayX11Screens[kPsychMaxPossibleDisplays];
static psych_bool               displayCursorHidden[kPsychMaxPossibleDisplays];
static XRRScreenResources*      displayX11ScreenResources[kPsychMaxPossibleDisplays];

// XInput-2 extension data per display:
static int                      xi_opcode = 0, xi_event = 0, xi_error = 0;
static int                      xinput_ndevices[kPsychMaxPossibleDisplays];
static XIDeviceInfo*            xinput_info[kPsychMaxPossibleDisplays];

// X11 has a different - and much more powerful and flexible - concept of displays than OS-X or Windows:
// One can have multiple X11 connections to different logical displays. A logical display corresponds
// to a specific X-Server. This X-Server could run on the same machine as Matlab+PTB or on a different
// machine connected via network somewhere in the building or the world. A single machine can even run
// multiple X-Servers. Each display itself can consist of multiple screens. Each screen represents
// a single physical display device. E.g., a dual-head gfx-adaptor could be driven by a single X-Server and have
// two screens for each physical output. A single X-Server could also drive multiple different gfx-cards
// and therefore have many screens. A Linux render-cluster could consist of multiple independent machines,
// each with multiple screens aka gfx heads connected to each machine (aka X11 display).
//
// By default, PTB just connects to the same display as the one that Matlab is running on and tries to
// detect and enumerate all physical screens connected to that display. The default display is set either
// via Matlab command option "-display" or via the Shell environment variable $DISPLAY. Typically, it
// is simply $DISPLAY=:0.0, which means the local gfx-adaptor attached to the machine the user is logged into.
//
// If a user wants to make use of other displays than the one Matlab is running on, (s)he can set the
// environment variable $PSYCHTOOLBOX_DISPLAYS to a list of all requested displays. PTB will then try
// to connect to each of the listed displays, enumerate all attached screens and build its list of
// available screens as a merge of all screens of all displays.
// E.g., export PSYCHTOOLBOX_DISPLAYS=":0.0,kiwi.kyb.local:0.0,coriander.kyb.local:0.0" would enumerate
// all screens of all gfx-adaptors on the local machine ":0.0", and the network connected machines
// "kiwi.kyb.local" and "coriander.kyb.local".
//
// Possible applications: Multi-display setups on Linux, possibly across machines, e.g., render-clusters
// Weird experiments with special setups. Show stimulus on display 1, query mouse or keyboard from
// different machine... 

static int x11_errorval = 0;
static int x11_errorbase = 0;
static int (*x11_olderrorhandler)(Display*, XErrorEvent*);

//file local functions
void InitCGDisplayIDList(void);
void PsychLockScreenSettings(int screenNumber);
void PsychUnlockScreenSettings(int screenNumber);
psych_bool PsychCheckScreenSettingsLock(int screenNumber);
//psych_bool PsychGetCGModeFromVideoSetting(CFDictionaryRef *cgMode, PsychScreenSettingsType *setting);
void InitPsychtoolboxKernelDriverInterface(void);

// Error callback handler for X11 errors:
static int x11VidModeErrorHandler(Display* dis, XErrorEvent* err)
{
  // If x11_errorbase not yet setup, simply return and ignore this error:
  if (x11_errorbase == 0) return(0);

  // Setup: Check if its an XVidMode-Error - the only one we do handle.
  if (err->error_code >=x11_errorbase && err->error_code < x11_errorbase + XF86VidModeNumberErrors ||
      err->error_code == BadValue) {
    // We caused some error. Set error flag:
    x11_errorval = 1;
  }

  // Done.
  return(0);
}

//Initialization functions
void InitializePsychDisplayGlue(void)
{
	static psych_bool firstTime = TRUE;
    int i;
    
    //init the display settings flags.
    for(i=0;i<kPsychMaxPossibleDisplays;i++){
        displayLockSettingsFlags[i]=FALSE;
        displayOriginalCGSettingsValid[i]=FALSE;
        displayOverlayedCGSettingsValid[i]=FALSE;
	displayCursorHidden[i]=FALSE;
	displayBeampositionHealthy[i]=TRUE;
	displayX11ScreenResources[i] = NULL;
	xinput_ndevices[i]=0;
	xinput_info[i]=NULL;
    }

    has_xrandr_1_2 = FALSE;
    has_xrandr_1_3 = FALSE;
    
	if (firstTime) {
		firstTime = FALSE;
		
		// We must initialize XLib for multi-threaded operations / access on first
		// call:
		// TODO FIXME: We can only do this on Octave for now, not on Matlab!
		// Matlab uses XLib long before we get a chance to get here, but XInitThreads()
		// must be called as very first XLib function after process startup or bad things
		// will happen! So, we can't call it...
		// Because some system configurations can't handle multi-threaded x at all,
		// we allow users to opt-out of this if they define an environment variable
		// PSYCHTOOLBOX_SINGLETHREADEDX.
		#ifdef PTBOCTAVE3MEX
		if (NULL == getenv("PSYCHTOOLBOX_SINGLETHREADEDX")) XInitThreads();
		#endif
	}

    //init the list of Core Graphics display IDs.
    InitCGDisplayIDList();

    // Attach to kernel-level Psychtoolbox graphics card interface driver if possible
    // *and* allowed by settings, setup all relevant mappings:
    InitPsychtoolboxKernelDriverInterface();
}

static void InitXInputExtensionForDisplay(CGDirectDisplayID dpy, int idx)
{
  int major, minor;
  int rc, i;

  // XInputExtension supported? If so do basic init:
  if (!XQueryExtension(dpy, "XInputExtension", &xi_opcode, &xi_event, &xi_error)) {
    printf("PTB-WARNING: XINPUT1/2 extension unsupported. Will only be able to handle one mouse and mouse cursor.\n");
    goto out;
  }

  // XInput V 2.0 or later supported?
  major = 2;
  minor = 0;
  rc = XIQueryVersion(dpy, &major, &minor);
  if (rc == BadRequest) {
    printf("PTB-WARNING: No XInput-2 support. Server supports version %d.%d only.\n", major, minor);
    printf("PTB-WARNING: XINPUT1/2 extension unsupported. Will only be able to handle one mouse and mouse cursor.\n");
    goto out;
  } else if (rc != Success) {
    printf("PTB-ERROR: Internal error during XInput-2 extension init sequence! This is a bug in Xlib!\n");
    printf("PTB-WARNING: XINPUT1/2 extension unsupported. Will only be able to handle one mouse and mouse cursor.\n");
    goto out;
  }

  // printf("PsychHID: INFO: XI2 supported. Server provides version %d.%d.\n", major, minor);

  // Enumerate all XI2 input devices for this x-display:
  xinput_info[idx] = XIQueryDevice(dpy, XIAllDevices, &xinput_ndevices[idx]);

out:
  return;
}

static void GetRandRScreenConfig(CGDirectDisplayID dpy, int idx)
{
  int major, minor;
  int o, num_crtcs, isPrimary, crtcid;
  int primaryOutput = -1, primaryCRTC = -1;
  int crtcs[100];

  // Preinit to "undefined":
  displayX11ScreenResources[idx] = NULL;

  // XRandR extension supported? If so do basic init:
  if (!XRRQueryExtension(dpy, &xr_event, &xr_error) ||
      !XRRQueryVersion(dpy, &major, &minor)) {
    printf("PTB-WARNING: XRandR extension unsupported. Display infos and configuration functions will be very limited!\n");
    return;
  }

  // Detect version of XRandR:
  if (major > 1 || (major == 1 && minor >= 2)) has_xrandr_1_2 = TRUE;
  if (major > 1 || (major == 1 && minor >= 3)) has_xrandr_1_3 = TRUE;

  // Fetch current screen configuration info for this screen and display:
  Window root = RootWindow(dpy, displayX11Screens[idx]);
  XRRScreenResources* res = XRRGetScreenResourcesCurrent(dpy, root);
  displayX11ScreenResources[idx] = res;
  if (NULL == res) {
    printf("PTB-WARNING: Could not query configuration of x-screen %i on display %s. Display infos and configuration will be very limited.\n",
	   displayX11Screens[idx], DisplayString(dpy));
    return;
  }

  if (!has_xrandr_1_2) {
    printf("PTB-WARNING: XRandR version 1.2 unsupported! Could not query useful info for x-screen %i on display %s. Infos and configuration will be very limited.\n",
	   displayX11Screens[idx], DisplayString(dpy));
    return;
  }

  // Iterate over all outputs for this screen:
  for (o = 0; o < res->noutput; o++) {
    XRROutputInfo *output_info = XRRGetOutputInfo(dpy, res, res->outputs[o]);
    if (!output_info) {
      printf("PTB-WARNING: Could not get output info for %i'th output of screen %i [display %s]!\n", o, displayX11Screens[idx], DisplayString(dpy));
      continue;
    }

    // Get info about this output:
    if (has_xrandr_1_3 && (XRRGetOutputPrimary(dpy, root) > 0)) {
      isPrimary = (XRRGetOutputPrimary(dpy, root) == output_info->crtc) ? 1 : 0;
    }
    else {
      isPrimary = -1;
    }

    for (crtcid = 0; crtcid < res->ncrtc; crtcid++) {
	if (res->crtcs[crtcid] == output_info->crtc) break;
    }
    if (crtcid == res->ncrtc) crtcid = -1;

    // Store crtc for this output:
    crtcs[o] = crtcid;

    printf("PTB-INFO: Display '%s' : Screen %i : Output %i [%s]: %s : ",
	   DisplayString(dpy), displayX11Screens[idx], o, (const char*) output_info->name, (isPrimary > -1) ? ((isPrimary == 1) ? "Primary output" : "Secondary output") : "Unknown output");
    printf("%s : CRTC %i [XID %i]\n", (output_info->connection == RR_Connected) ? "Connected" : "Offline", crtcid, (int) output_info->crtc);

    if (isPrimary > 0) {
	primaryOutput = o;
	primaryCRTC = crtcid;
    }
  }

  // Found a defined primary output?
  if (primaryOutput == -1) {
    // Could not find primary output -- none defined. Use first connected
    // output as primary output:
    for (o = 0; o < res->noutput; o++) {
      XRROutputInfo *output_info = XRRGetOutputInfo(dpy, res, res->outputs[o]);
      if (output_info && output_info->connection == RR_Connected) {
	primaryOutput = o;
	primaryCRTC = crtcs[o];
	break;
      }
    }
    // Still undefined? Use first output as primary output:
    if (primaryOutput == -1) {
	primaryOutput = 0;
	primaryCRTC = crtcs[0];
    }
  }

  printf("PTB-INFO: Display '%s' : Screen %i : Primary output is %i with crtc %i.\n", DisplayString(dpy), displayX11Screens[idx], primaryOutput, primaryCRTC);

  return;
}

void InitCGDisplayIDList(void)
{  
  int major, minor;
  int rc, i, j, k, count, scrnid;
  char* ptbdisplays = NULL;
  char displayname[1000];
  CGDirectDisplayID x11_dpy = NULL;
 
  // NULL-out array of displays:
  for(i=0;i<kPsychMaxPossibleDisplays;i++) displayCGIDs[i]=NULL;

  // Initial count of screens is zero:
  numDisplays = 0;

  // Multiple X11 display specifier strings provided in the environment variable
  // $PSYCHTOOLBOX_DISPLAYS? If so, we connect to all of them and enumerate all
  // available screens on them.
  ptbdisplays = getenv("PSYCHTOOLBOX_DISPLAYS");
  if (ptbdisplays) {
    // Displays explicitely specified. Parse the string and connect to all of them:
    j=0;
    for (i=0; i<=strlen(ptbdisplays) && j<1000; i++) {
      // Accepted separators are ',', '"', white-space and end of string...
      if (ptbdisplays[i]==',' || ptbdisplays[i]=='"' || ptbdisplays[i]==' ' || i==strlen(ptbdisplays)) {
	// Separator or end of string detected. Try to connect to display:
	displayname[j]=0;
	printf("PTB-INFO: Trying to connect to X-Display %s ...", displayname);

	x11_dpy = XOpenDisplay(displayname);
	if (x11_dpy == NULL) {
	  // Failed.
	  printf(" ...Failed! Skipping this display...\n");
	}
	else {
	  // Query number of available screens on this X11 display:
	  count=ScreenCount(x11_dpy);
	  scrnid=0;

	  // Set the screenNumber --> X11 display mappings up:
	  for (k=numDisplays; (k<numDisplays + count) && (k<kPsychMaxPossibleDisplays); k++) {
	    if (k == numDisplays) {
		// 1st entry for this x-display: Init XInput2 extension for it:
		InitXInputExtensionForDisplay(x11_dpy, numDisplays);
	    } else {
		// Successive entry. Copy info from 1st entry:
		xinput_info[k] = xinput_info[numDisplays];
		xinput_ndevices[k] = xinput_ndevices[numDisplays];
	    }

	    // Mapping of logical screenNumber to X11 Display:
	    displayCGIDs[k]= x11_dpy;
	    // Mapping of logical screenNumber to X11 screenNumber for X11 Display:
	    displayX11Screens[k]=scrnid++;

	    // Get all relevant screen config info and cache it internally:
	    GetRandRScreenConfig(x11_dpy, k);
	  }

	  printf(" ...success! Added %i new physical display screens of %s as PTB screens %i to %i.\n",
		 scrnid, displayname, numDisplays, k-1);

	  // Update total count:
	  numDisplays = k;
	}

	// Reset idx:
	j=0;
      }
      else {
	// Add character to display name:
	displayname[j++]=ptbdisplays[i];
      }
    }
    
    // At least one screen enumerated?
    if (numDisplays < 1) {
      // We're screwed :(
      PsychErrorExitMsg(PsychError_internal, "FATAL ERROR: Couldn't open any X11 display connection to any X-Server!!!");
    }
  }
  else {
    // User didn't setup env-variable with any special displays. We just use
    // the default $DISPLAY or -display of Matlab:
    x11_dpy = XOpenDisplay(NULL);
    if (x11_dpy == NULL) {
      // We're screwed :(
      PsychErrorExitMsg(PsychError_internal, "FATAL ERROR: Couldn't open default X11 display connection to X-Server!!!");
    }
    
    // Query number of available screens on this X11 display:
    count=ScreenCount(x11_dpy);

    InitXInputExtensionForDisplay(x11_dpy, 0);

    // Set the screenNumber --> X11 display mappings up:
    for (i=0; i<count && i<kPsychMaxPossibleDisplays; i++) {
	displayCGIDs[i]= x11_dpy;
	displayX11Screens[i]=i;
	xinput_info[i] = xinput_info[0];
	xinput_ndevices[i] = xinput_ndevices[0];

	// Get all relevant screen config info and cache it internally:
	GetRandRScreenConfig(x11_dpy, i);
    }
    numDisplays=i;
  }

  if (numDisplays>1) printf("PTB-Info: A total of %i physical X-Windows display screens is available for use.\n", numDisplays);

  // Initialize screenId -> GPU headId mapping:
  PsychInitScreenToHeadMappings(numDisplays);

  return;
}

void PsychCleanupDisplayGlue(void)
{
	CGDirectDisplayID dpy, last_dpy;
	int i;

	last_dpy = NULL;
	// Go trough full screen list:
	for (i=0; i < PsychGetNumDisplays(); i++) {
	  // Get display-ptr for this screen:
	  PsychGetCGDisplayIDFromScreenNumber(&dpy, i);

	  // Did we close this connection already (dpy==last_dpy)?
	  if (dpy != last_dpy) {
	    // Nope. Keep track of it...
	    last_dpy=dpy;
	    // ...and close display connection to X-Server:
	    XCloseDisplay(dpy);

	    // Release actual xinput info list for this x11 display connection:
	    if (xinput_info[i]) {
		XIFreeDeviceInfo(xinput_info[i]);
	    }
	  }

	  // NULL-Out xinput extension data:
	  xinput_info[i] = NULL;
	  xinput_ndevices[i] = 0;
	}

	// All connections should be closed now. We can't NULL-out the display list, but
	// Matlab will flush the Screen - Mexfile anyway...
	return;
}

XIDeviceInfo* PsychGetInputDevicesForScreen(int screenNumber, int* nDevices)
{
  if(screenNumber >= numDisplays) PsychErrorExit(PsychError_invalidScumber);
  if (nDevices) *nDevices = xinput_ndevices[screenNumber];
  return(xinput_info[screenNumber]);
}

int PsychGetXScreenIdForScreen(int screenNumber)
{
  if(screenNumber>=numDisplays) PsychErrorExit(PsychError_invalidScumber);
  return(displayX11Screens[screenNumber]);
}

void PsychGetCGDisplayIDFromScreenNumber(CGDirectDisplayID *displayID, int screenNumber)
{
    if(screenNumber>=numDisplays) PsychErrorExit(PsychError_invalidScumber);
    *displayID=displayCGIDs[screenNumber];
}


/*  About locking display settings:

    SCREENOpenWindow and SCREENOpenOffscreenWindow  set the lock when opening  windows and 
    SCREENCloseWindow unsets upon the close of the last of a screen's windows. PsychSetVideoSettings checks for a lock
    before changing the settings.  Anything (SCREENOpenWindow or SCREENResolutions) which attemps to change
    the display settings should report that attempts to change a dipslay's settings are not allowed when its windows are open.
    
    PsychSetVideoSettings() may be called by either SCREENOpenWindow or by Resolutions().  If called by Resolutions it both sets the video settings
    and caches the video settings so that subsequent calls to OpenWindow can use the cached mode regardless of whether interceding calls to OpenWindow
    have changed the display settings or reverted to the virgin display settings by closing.  SCREENOpenWindow should thus invoke the cached mode
    settings if they have been specified and not current actual display settings.  
    
*/    
void PsychLockScreenSettings(int screenNumber)
{
    displayLockSettingsFlags[screenNumber]=TRUE;
}

void PsychUnlockScreenSettings(int screenNumber)
{
    displayLockSettingsFlags[screenNumber]=FALSE;
}

psych_bool PsychCheckScreenSettingsLock(int screenNumber)
{
    return(displayLockSettingsFlags[screenNumber]);
}


/* Because capture and lock will always be used in conjuction, capture calls lock, and SCREENOpenWindow must only call Capture and Release */
void PsychCaptureScreen(int screenNumber)
{
    CGDisplayErr  error=0;
    
    if(screenNumber>=numDisplays) PsychErrorExit(PsychError_invalidScumber);

    // MK: We could do this to get exclusive access to the X-Server, but i'm too
    // scared of doing it at the moment:
    // XGrabServer(displayCGIDs[screenNumber]);

    if(error) PsychErrorExitMsg(PsychError_internal, "Unable to capture display");
    PsychLockScreenSettings(screenNumber);
}

/*
    PsychReleaseScreen()    
*/
void PsychReleaseScreen(int screenNumber)
{	
    CGDisplayErr  error=0;
    
    if(screenNumber>=numDisplays) PsychErrorExit(PsychError_invalidScumber);
    // MK: We could do this to release exclusive access to the X-Server, but i'm too
    // scared of doing it at the moment:
    // XUngrabServer(displayCGIDs[screenNumber]);

    // On Linux we restore the original display settings of the to be released screen:
    PsychRestoreScreenSettings(screenNumber);
    if(error) PsychErrorExitMsg(PsychError_internal, "Unable to release display");
    PsychUnlockScreenSettings(screenNumber);
}

psych_bool PsychIsScreenCaptured(int screenNumber)
{
    return(PsychCheckScreenSettingsLock(screenNumber));
}    


//Read display parameters.
/*
    PsychGetNumDisplays()
    Get the number of video displays connected to the system.
*/

int PsychGetNumDisplays(void)
{
    return((int) numDisplays);
}

void PsychGetScreenDepths(int screenNumber, PsychDepthType *depths)
{
  int* x11_depths;
  int  i, count;

  if(screenNumber>=numDisplays) PsychErrorExitMsg(PsychError_internal, "screenNumber is out of range"); //also checked within SCREENPixelSizes

  x11_depths = XListDepths(displayCGIDs[screenNumber], PsychGetXScreenIdForScreen(screenNumber), &count);
  if (depths && count>0) {
    // Query successful: Add all values to depth struct:
    for(i=0; i<count; i++) PsychAddValueToDepthStruct(x11_depths[i], depths);
    XFree(x11_depths);
  }
  else {
    // Query failed: Assume at least 32 bits is available.
    printf("PTB-WARNING: Couldn't query available display depths values! Returning a made up list...\n");
    fflush(NULL);
    PsychAddValueToDepthStruct(32, depths);
    PsychAddValueToDepthStruct(24, depths);
    PsychAddValueToDepthStruct(16, depths); 
  }
}

/*   PsychGetAllSupportedScreenSettings()
 *
 *	 Queries the display system for a list of all supported display modes, ie. all valid combinations
 *	 of resolution, pixeldepth and refresh rate. Allocates temporary arrays for storage of this list
 *	 and returns it to the calling routine. This function is basically only used by Screen('Resolutions').
 */
int PsychGetAllSupportedScreenSettings(int screenNumber, long** widths, long** heights, long** hz, long** bpp)
{
//    int i, rc, numPossibleModes;
//    DEVMODE result;
//    long tempWidth, tempHeight, currentFrequency, tempFrequency, tempDepth;
//
//    if(screenNumber>=numDisplays) PsychErrorExit(PsychError_invalidScumber);
//
//	// First pass: How many modes are supported?
//	i=-1;
//	do {
//        result.dmSize = sizeof(DEVMODE);
//        result.dmDriverExtra = 0;
//        rc = EnumDisplaySettings(PsychGetDisplayDeviceName(screenNumber), i, &result);
//        i++;
//	} while (rc!=0);
//
//    // Get a list of avialable modes for the specified display:
//    numPossibleModes= i;
//	
//	// Allocate output arrays: These will get auto-released at exit
//	// from Screen():
//	*widths = (long*) PsychMallocTemp(numPossibleModes * sizeof(int));
//	*heights = (long*) PsychMallocTemp(numPossibleModes * sizeof(int));
//	*hz = (long*) PsychMallocTemp(numPossibleModes * sizeof(int));
//	*bpp = (long*) PsychMallocTemp(numPossibleModes * sizeof(int));
//	
//	// Fetch modes and store into arrays:
//    for(i=0; i < numPossibleModes; i++) {
//        result.dmSize = sizeof(DEVMODE);
//        result.dmDriverExtra = 0;
//        rc = EnumDisplaySettings(PsychGetDisplayDeviceName(screenNumber), i, &result);
//		(*widths)[i] = (long)  result.dmPelsWidth;
//		(*heights)[i] = (long)  result.dmPelsHeight;
//		(*hz)[i] = (long) result.dmDisplayFrequency;
//		(*bpp)[i] = (long) result.dmBitsPerPel;
//    }
//
//	return(numPossibleModes);

	// FIXME: Not yet implemented!
	return(0);
}

/*
    static PsychGetCGModeFromVideoSettings()
   
*/
psych_bool PsychGetCGModeFromVideoSetting(CFDictionaryRef *cgMode, PsychScreenSettingsType *setting)
{
  // Dummy assignment:
  *cgMode = 1;
  return(TRUE);
}


/*
    PsychCheckVideoSettings()
    
    Check all available video display modes for the specified screen number and return true if the 
    settings are valid and false otherwise.
*/
psych_bool PsychCheckVideoSettings(PsychScreenSettingsType *setting)
{
        CFDictionaryRef cgMode;       
        return(PsychGetCGModeFromVideoSetting(&cgMode, setting));
}

/*
    PsychGetScreenDepth()
    
    The caller must allocate and initialize the depth struct. 
*/
void PsychGetScreenDepth(int screenNumber, PsychDepthType *depth)
{    
  if(screenNumber>=numDisplays) PsychErrorExitMsg(PsychError_internal, "screenNumber is out of range"); //also checked within SCREENPixelSizes
  PsychAddValueToDepthStruct(DefaultDepth(displayCGIDs[screenNumber], PsychGetXScreenIdForScreen(screenNumber)), depth);
}

int PsychGetScreenDepthValue(int screenNumber)
{
    PsychDepthType	depthStruct;
    
    PsychInitDepthStruct(&depthStruct);
    PsychGetScreenDepth(screenNumber, &depthStruct);
    return(PsychGetValueFromDepthStruct(0,&depthStruct));
}


float PsychGetNominalFramerate(int screenNumber)
{
#ifdef USE_VIDMODEEXTS

  // Information returned by the XF86VidModeExtension:
  XF86VidModeModeLine mode_line;  // The mode line of the current video mode.
  int dot_clock;                  // The RAMDAC / TDMS pixel clock frequency.

  // We start with a default vrefresh of zero, which means "couldn't query refresh from OS":
  float vrefresh = 0;

  if(screenNumber>=numDisplays)
    PsychErrorExitMsg(PsychError_internal, "screenNumber passed to PsychGetScreenDepths() is out of range"); 

  if (!XF86VidModeSetClientVersion(displayCGIDs[screenNumber])) {
    // Failed to use VidMode-Extension. We just return a vrefresh of zero.
    return(0);
  }

  // Query vertical refresh rate. If it fails we default to the last known good value...
  if (XF86VidModeGetModeLine(displayCGIDs[screenNumber], PsychGetXScreenIdForScreen(screenNumber), &dot_clock, &mode_line)) {
    // Vertical refresh rate is: RAMDAC pixel clock / width of a scanline in clockcylces /
    // number of scanlines per videoframe.
    vrefresh = (((dot_clock * 1000) / mode_line.htotal) * 1000) / mode_line.vtotal;

    // Divide vrefresh by 1000 to get real Hz - value:
    vrefresh = vrefresh / 1000.0f;

    // Definitions from xserver's hw/xfree86/common/xf86str.h
    // V_INTERLACE	= 0x0010,
    // V_DBLSCAN	= 0x0020,

    // Doublescan mode? If so, divide vrefresh by 2:
    if (mode_line.flags & 0x0020) vrefresh /= 2;

    // Interlaced mode? If so, multiply vrefresh by 2:
    if (mode_line.flags & 0x0010) vrefresh *= 2;
  }

  // Done.
  return(vrefresh);
#else
  return(0);
#endif
}

float PsychSetNominalFramerate(int screenNumber, float requestedHz)
{
#ifdef USE_VIDMODEEXTS

  // Information returned by/sent to the XF86VidModeExtension:
  XF86VidModeModeLine mode_line;  // The mode line of the current video mode.
  int dot_clock;                  // The RAMDAC / TDMS pixel clock frequency.
  int rc;
  int event_base;

  // We start with a default vrefresh of zero, which means "couldn't query refresh from OS":
  float vrefresh = 0;

  if(screenNumber>=numDisplays)
    PsychErrorExitMsg(PsychError_internal, "screenNumber is out of range"); 

  if (!XF86VidModeSetClientVersion(displayCGIDs[screenNumber])) {
    // Failed to use VidMode-Extension. We just return a vrefresh of zero.
    return(0);
  }

  if (!XF86VidModeQueryExtension(displayCGIDs[screenNumber], &event_base, &x11_errorbase)) {
    // Failed to use VidMode-Extension. We just return a vrefresh of zero.
    return(0);
  }

  // Attach our error callback handler and reset error-state:
  x11_errorval = 0;
  x11_olderrorhandler = XSetErrorHandler(x11VidModeErrorHandler);

  // Step 1: Query current dotclock and modeline:
  if (!XF86VidModeGetModeLine(displayCGIDs[screenNumber], PsychGetXScreenIdForScreen(screenNumber), &dot_clock, &mode_line)) {
    // Restore default error handler:
    XSetErrorHandler(x11_olderrorhandler);

    PsychErrorExitMsg(PsychError_internal, "Failed to query video dotclock and modeline!"); 
  }

  // Step 2: Calculate updated modeline:
  if (requestedHz > 10) {
    // Step 2-a: Given current dot-clock and modeline and requested vrefresh, compute
    // modeline for closest possible match:
    requestedHz*=1000.0f;
    vrefresh = (((dot_clock * 1000) / mode_line.htotal) * 1000) / requestedHz;
    
    // Assign it to closest modeline setting:
    mode_line.vtotal = (int)(vrefresh + 0.5f);
  }
  else {
    // Step 2-b: Delta mode. requestedHz represents a direct integral offset
    // to add or subtract from current modeline setting:
    mode_line.vtotal+=(int) requestedHz;
  }

  // Step 3: Try to set new modeline:
  if (!XF86VidModeModModeLine(displayCGIDs[screenNumber], PsychGetXScreenIdForScreen(screenNumber), &mode_line)) {
    // Restore default error handler:
    XSetErrorHandler(x11_olderrorhandler);

    // Invalid modeline? Signal this:
    return(-1);
  }

  // We synchronize and wait for X-Request completion. If the modeline was invalid,
  // this will trigger an invocation of our errorhandler, which in turn will
  // set the x11_errorval to a non-zero value:
  XSync(displayCGIDs[screenNumber], FALSE);
  
  // Restore default error handler:
  XSetErrorHandler(x11_olderrorhandler);

  // Check for error:
  if (x11_errorval) {
    // Failed to set new mode! Must be invalid. We return -1 to signal this:
    return(-1);
  }

  // No error...

  // Step 4: Query new settings and return them:
  vrefresh = PsychGetNominalFramerate(screenNumber);

  // Done.
  return(vrefresh);
#else
  return(0);
#endif
}

/* Returns the physical display size as reported by X11: */
void PsychGetDisplaySize(int screenNumber, int *width, int *height)
{
    if(screenNumber>=numDisplays)
        PsychErrorExitMsg(PsychError_internal, "screenNumber passed to PsychGetDisplaySize() is out of range");
    *width = (int) XDisplayWidthMM(displayCGIDs[screenNumber], PsychGetXScreenIdForScreen(screenNumber));
    *height = (int) XDisplayHeightMM(displayCGIDs[screenNumber], PsychGetXScreenIdForScreen(screenNumber));
}

void PsychGetScreenSize(int screenNumber, long *width, long *height)
{
  if(screenNumber>=numDisplays) PsychErrorExitMsg(PsychError_internal, "screenNumber passed to PsychGetScreenDepths() is out of range"); 
  *width=XDisplayWidth(displayCGIDs[screenNumber], PsychGetXScreenIdForScreen(screenNumber));
  *height=XDisplayHeight(displayCGIDs[screenNumber], PsychGetXScreenIdForScreen(screenNumber));
}


void PsychGetGlobalScreenRect(int screenNumber, double *rect)
{
  // Create an empty rect:
  PsychMakeRect(rect, 0, 0, 1, 1);
  // Fill it with meaning by PsychGetScreenRect():
  PsychGetScreenRect(screenNumber, rect);
}


void PsychGetScreenRect(int screenNumber, double *rect)
{
    long width, height; 

    PsychGetScreenSize(screenNumber, &width, &height);
    rect[kPsychLeft]=0;
    rect[kPsychTop]=0;
    rect[kPsychRight]=(int)width;
    rect[kPsychBottom]=(int)height; 
} 


PsychColorModeType PsychGetScreenMode(int screenNumber)
{
    PsychDepthType depth;
        
    PsychInitDepthStruct(&depth);
    PsychGetScreenDepth(screenNumber, &depth);
    return(PsychGetColorModeFromDepthStruct(&depth));
}


/*
    Its probably better to read this directly from the CG renderer info than to infer it from the pixel size
*/	
int PsychGetNumScreenPlanes(int screenNumber)
{    
    return((PsychGetScreenDepthValue(screenNumber)>24) ? 4 : 3);
}



/*
	This is a place holder for a function which uncovers the number of dacbits.  To be filled in at a later date.
	If you know that your card supports >8 then you can fill that in the PsychtPreferences and the psychtoolbox
	will act accordingly.
	
	There seems to be no way to uncover the dacbits programatically.  According to apple CoreGraphics
	sends a 16-bit word and the driver throws out whatever it chooses not to use.
		
	For now we just use 8 to avoid false precision.  
	
	If we can uncover the video card model then  we can implement a table lookup of video card model to number of dacbits.  
*/
int PsychGetDacBitsFromDisplay(int screenNumber)
{
	return(8);
}



/*
    PsychGetVideoSettings()
    
    Fills a structure describing the screen settings such as x, y, depth, frequency, etc.
    
    Consider inverting the calling sequence so that this function is at the bottom of call hierarchy.  
*/ 
void PsychGetScreenSettings(int screenNumber, PsychScreenSettingsType *settings)
{
    settings->screenNumber=screenNumber;
    PsychGetScreenRect(screenNumber, settings->rect);
    PsychInitDepthStruct(&(settings->depth));
    PsychGetScreenDepth(screenNumber, &(settings->depth));
    settings->mode=PsychGetColorModeFromDepthStruct(&(settings->depth));
    settings->nominalFrameRate=PsychGetNominalFramerate(screenNumber);
    //settings->dacbits=PsychGetDacBits(screenNumber);
}




//Set display parameters

/*
    PsychSetScreenSettings()
	
    Accept a PsychScreenSettingsType structure holding a video mode and set the display mode accordingly.
    
    If we can not change the display settings because of a lock (set by open window or close window) then return false.
    
    SCREENOpenWindow should capture the display before it sets the video mode.  If it doesn't, then PsychSetVideoSettings will
    detect that and exit with an error.  SCREENClose should uncapture the display. 
    
    The duties of SCREENOpenWindow are:
    -Lock the screen which serves the purpose of preventing changes in video setting with open Windows.
    -Capture the display which gives the application synchronous control of display parameters.
    
    TO DO: for 8-bit palletized mode there is probably more work to do.  
      
*/

psych_bool PsychSetScreenSettings(psych_bool cacheSettings, PsychScreenSettingsType *settings)
{
    CFDictionaryRef 		cgMode;
    psych_bool 			isValid, isCaptured;
    CGDisplayErr 		error;

    //get the display IDs.  Maybe we should consolidate this out of these functions and cache the IDs in a file static
    //variable, since basicially every core graphics function goes through this deal.    
    if(settings->screenNumber>=numDisplays)
        PsychErrorExitMsg(PsychError_internal, "screenNumber passed to PsychSetScreenSettings() is out of range");

    //Check for a lock which means onscreen or offscreen windows tied to this screen are currently open.
    // MK Disabled: if(PsychCheckScreenSettingsLock(settings->screenNumber)) return(false);  //calling function should issue an error for attempt to change display settings while windows were open.
    
    //store the original display mode if this is the first time we have called this function.  The psychtoolbox will disregard changes in 
    //the screen state made through the control panel after the Psychtoolbox was launched. That is, OpenWindow will by default continue to 
    //open windows with finder settings which were in place at the first call of OpenWindow.  That's not intuitive, but not much of a problem
    //either. 
    if(!displayOriginalCGSettingsValid[settings->screenNumber]){
      displayOriginalCGSettings[settings->screenNumber]= 1; // FIXME!!! CGDisplayCurrentMode(displayCGIDs[settings->screenNumber]);
      displayOriginalCGSettingsValid[settings->screenNumber]=TRUE;
    }
    
    //Find core graphics video settings which correspond to settings as specified withing by an abstracted psychsettings structure.  
    isValid=PsychGetCGModeFromVideoSetting(&cgMode, settings);
    if(!isValid){
        PsychErrorExitMsg(PsychError_internal, "Attempt to set invalid video settings"); 
        //this is an internal error because the caller is expected to check first. 
    }
    
    //If the caller passed cache settings (then it is SCREENResolutions) and we should cache the current video mode settings for this display.  These
    //are cached in the form of CoreGraphics settings and not Psychtoolbox video settings.  The only caller which should pass a set cache flag is 
    //SCREENResolutions
    if(cacheSettings){
        displayOverlayedCGSettings[settings->screenNumber]=cgMode;
        displayOverlayedCGSettingsValid[settings->screenNumber]=TRUE;
    }
    
    //Check to make sure that this display is captured, which OpenWindow should have done.  If it has not been done, then exit with an error.  
    isCaptured=PsychIsScreenCaptured(settings->screenNumber);
    if(!isCaptured) PsychErrorExitMsg(PsychError_internal, "Attempt to change video settings without capturing the display");
        
    //Change the display mode.   
    // FIXME: Not yet implemented.
    
    return(true);
}

/*
    PsychRestoreVideoSettings()
    
    Restores video settings to the state set by the finder.  Returns TRUE if the settings can be restored or false if they 
    can not be restored because a lock is in effect, which would mean that there are still open windows.    
    
*/
psych_bool PsychRestoreScreenSettings(int screenNumber)
{
    psych_bool 			isCaptured;
    CGDisplayErr 		error=0;


    if(screenNumber>=numDisplays)
        PsychErrorExitMsg(PsychError_internal, "screenNumber passed to PsychGetScreenDepths() is out of range"); //also checked within SCREENPixelSizes

    //Check for a lock which means onscreen or offscreen windows tied to this screen are currently open.
    // if(PsychCheckScreenSettingsLock(screenNumber)) return(false);  //calling function will issue error for attempt to change display settings while windows were open.
    
    //Check to make sure that the original graphics settings were cached.  If not, it means that the settings were never changed, so we can just
    //return true. 
    if(!displayOriginalCGSettingsValid[screenNumber])
        return(true);
    
    //Check to make sure that this display is captured, which OpenWindow should have done.  If it has not been done, then exit with an error.  
    isCaptured=PsychIsScreenCaptured(screenNumber);
    if(!isCaptured) PsychErrorExitMsg(PsychError_internal, "Attempt to change video settings without capturing the display");
    
    // FIXME: Not yet implemented...
    return(true);
}


void PsychHideCursor(int screenNumber, int deviceIdx)
{
  // Static "Cursor" object which defines a completely transparent - and therefore invisible
  // X11 cursor for the mouse-pointer.
  static Cursor nullCursor = -1;

  // Check for valid screenNumber:
  if(screenNumber>=numDisplays) PsychErrorExitMsg(PsychError_internal, "screenNumber passed to PsychHideCursor() is out of range"); //also checked within SCREENPixelSizes

  // Cursor already hidden on screen? If so, nothing to do:
  if ((deviceIdx < 0) && displayCursorHidden[screenNumber]) return;

  // nullCursor already ready?
  if( nullCursor == (Cursor) -1 ) {
    // Create one:
    Pixmap cursormask;
    XGCValues xgc;
    GC gc;
    XColor dummycolour;

    cursormask = XCreatePixmap(displayCGIDs[screenNumber], RootWindow(displayCGIDs[screenNumber], PsychGetXScreenIdForScreen(screenNumber)), 1, 1, 1/*depth*/);
    xgc.function = GXclear;
    gc = XCreateGC(displayCGIDs[screenNumber], cursormask, GCFunction, &xgc );
    XFillRectangle(displayCGIDs[screenNumber], cursormask, gc, 0, 0, 1, 1 );
    dummycolour.pixel = 0;
    dummycolour.red   = 0;
    dummycolour.flags = 04;
    nullCursor = XCreatePixmapCursor(displayCGIDs[screenNumber], cursormask, cursormask, &dummycolour, &dummycolour, 0, 0 );
    XFreePixmap(displayCGIDs[screenNumber], cursormask );
    XFreeGC(displayCGIDs[screenNumber], gc );
  }

  if (deviceIdx < 0) {
	  // Attach nullCursor to our onscreen window:
	  XDefineCursor(displayCGIDs[screenNumber], RootWindow(displayCGIDs[screenNumber], PsychGetXScreenIdForScreen(screenNumber)), nullCursor );
	  XFlush(displayCGIDs[screenNumber]);
	  displayCursorHidden[screenNumber]=TRUE;
  } else {
	// XInput cursor: Master pointers only.
	int nDevices;
	XIDeviceInfo* indevs = PsychGetInputDevicesForScreen(screenNumber, &nDevices);

	// Sanity check:
	if (NULL == indevs) PsychErrorExitMsg(PsychError_user, "Sorry, your system does not support individual mouse pointers.");
	if (deviceIdx >= nDevices) PsychErrorExitMsg(PsychError_user, "Invalid 'mouseIndex' provided. No such cursor pointer.");
	if (indevs[deviceIdx].use != XIMasterPointer) PsychErrorExitMsg(PsychError_user, "Invalid 'mouseIndex' provided. No such master cursor pointer.");

	// Attach nullCursor to our onscreen window:
	XIDefineCursor(displayCGIDs[screenNumber], indevs[deviceIdx].deviceid, RootWindow(displayCGIDs[screenNumber], PsychGetXScreenIdForScreen(screenNumber)), nullCursor);
	XFlush(displayCGIDs[screenNumber]);
  }

  return;
}

void PsychShowCursor(int screenNumber, int deviceIdx)
{
  Cursor arrowCursor;

  // Check for valid screenNumber:
  if(screenNumber>=numDisplays) PsychErrorExitMsg(PsychError_internal, "screenNumber passed to PsychHideCursor() is out of range"); //also checked within SCREENPixelSizes

  if (deviceIdx < 0) {
	// Cursor not hidden on screen? If so, nothing to do:
	if(!displayCursorHidden[screenNumber]) return;

	// Reset to standard Arrow-Type cursor, which is a visible one.
	arrowCursor = XCreateFontCursor(displayCGIDs[screenNumber], 2);
	XDefineCursor(displayCGIDs[screenNumber], RootWindow(displayCGIDs[screenNumber], PsychGetXScreenIdForScreen(screenNumber)), arrowCursor);
	XFlush(displayCGIDs[screenNumber]);
	displayCursorHidden[screenNumber]=FALSE;
  } else {
	// XInput cursor: Master pointers only.
	int nDevices;
	XIDeviceInfo* indevs = PsychGetInputDevicesForScreen(screenNumber, &nDevices);

	// Sanity check:
	if (NULL == indevs) PsychErrorExitMsg(PsychError_user, "Sorry, your system does not support individual mouse pointers.");
	if (deviceIdx >= nDevices) PsychErrorExitMsg(PsychError_user, "Invalid 'mouseIndex' provided. No such cursor pointer.");
	if (indevs[deviceIdx].use != XIMasterPointer) PsychErrorExitMsg(PsychError_user, "Invalid 'mouseIndex' provided. No such master cursor pointer.");

	// Reset to standard Arrow-Type cursor, which is a visible one.
	arrowCursor = XCreateFontCursor(displayCGIDs[screenNumber], 2);
	XIDefineCursor(displayCGIDs[screenNumber], indevs[deviceIdx].deviceid, RootWindow(displayCGIDs[screenNumber], PsychGetXScreenIdForScreen(screenNumber)), arrowCursor);
	XFlush(displayCGIDs[screenNumber]);
  }
}

void PsychPositionCursor(int screenNumber, int x, int y, int deviceIdx)
{
  // Reposition the mouse cursor:
  if (deviceIdx < 0) {
	// Core protocol cursor:
	if (XWarpPointer(displayCGIDs[screenNumber], None, RootWindow(displayCGIDs[screenNumber], PsychGetXScreenIdForScreen(screenNumber)), 0, 0, 0, 0, x, y)==BadWindow) {
			  PsychErrorExitMsg(PsychError_internal, "Couldn't position the mouse cursor! (XWarpPointer() failed).");
	}
  } else {
	// XInput cursor: Master pointers only.
	int nDevices;
	XIDeviceInfo* indevs = PsychGetInputDevicesForScreen(screenNumber, &nDevices);

	// Sanity check:
	if (NULL == indevs) PsychErrorExitMsg(PsychError_user, "Sorry, your system does not support individual mouse pointers.");
	if (deviceIdx >= nDevices) PsychErrorExitMsg(PsychError_user, "Invalid 'mouseIndex' provided. No such cursor pointer.");
	if (indevs[deviceIdx].use != XIMasterPointer) PsychErrorExitMsg(PsychError_user, "Invalid 'mouseIndex' provided. No such master cursor pointer.");

	if (XIWarpPointer(displayCGIDs[screenNumber], indevs[deviceIdx].deviceid, None, RootWindow(displayCGIDs[screenNumber], PsychGetXScreenIdForScreen(screenNumber)), 0, 0, 0, 0, x, y)) {
			  PsychErrorExitMsg(PsychError_internal, "Couldn't position the mouse cursor! (XIWarpPointer() failed).");
	}
  }

  XFlush(displayCGIDs[screenNumber]);
}

/*
    PsychReadNormalizedGammaTable()
    
    TO DO: This should probably be changed so that the caller allocates the memory.
    TO DO: Adopt a naming convention which distinguishes between functions which allocate memory for return variables from those which do not.
            For example, PsychReadNormalizedGammaTable() vs. PsychGetNormalizedGammaTable().
    
*/
void PsychReadNormalizedGammaTable(int screenNumber, int *numEntries, float **redTable, float **greenTable, float **blueTable)
{
#ifdef USE_VIDMODEEXTS

  CGDirectDisplayID	cgDisplayID;
  static  float localRed[256], localGreen[256], localBlue[256];
  
  // The X-Windows hardware LUT has 3 tables for R,G,B, 256 slots each.
  // Each entry is a 16-bit word with the n most significant bits used for an n-bit DAC.
  psych_uint16	RTable[256];
  psych_uint16	GTable[256];
  psych_uint16	BTable[256];
  int     i;        

  // Query OS for gamma table:
  PsychGetCGDisplayIDFromScreenNumber(&cgDisplayID, screenNumber);
  XF86VidModeGetGammaRamp(cgDisplayID, PsychGetXScreenIdForScreen(screenNumber), 256, (unsigned short*) RTable, (unsigned short*) GTable, (unsigned short*) BTable);

  // Convert tables:Map 16-bit values into 0-1 normalized floats:
  *redTable=localRed; *greenTable=localGreen; *blueTable=localBlue;
  for (i=0; i<256; i++) localRed[i]   = ((float) RTable[i]) / 65535.0f;
  for (i=0; i<256; i++) localGreen[i] = ((float) GTable[i]) / 65535.0f;
  for (i=0; i<256; i++) localBlue[i]  = ((float) BTable[i]) / 65535.0f;

  // The LUT's always have 256 slots for the 8-bit framebuffer:
  *numEntries= 256;
#endif
  return;
}

unsigned int PsychLoadNormalizedGammaTable(int screenNumber, int numEntries, float *redTable, float *greenTable, float *blueTable)
{
#ifdef USE_VIDMODEEXTS

  CGDirectDisplayID	cgDisplayID;
  int     i;        
  psych_uint16	RTable[256];
  psych_uint16	GTable[256];
  psych_uint16	BTable[256];

  // Table must have 256 slots!
  if (numEntries!=256) PsychErrorExitMsg(PsychError_user, "Loadable hardware gamma tables must have 256 slots!");    
  
  // The X-Windows hardware LUT has 3 tables for R,G,B, 256 slots each.
  // Each entry is a 16-bit word with the n most significant bits used for an n-bit DAC.

  // Convert input table to X11 specific gammaTable:
  for (i=0; i<256; i++) RTable[i] = (int)(redTable[i]   * 65535.0f + 0.5f);
  for (i=0; i<256; i++) GTable[i] = (int)(greenTable[i] * 65535.0f + 0.5f);
  for (i=0; i<256; i++) BTable[i] = (int)(blueTable[i]  * 65535.0f + 0.5f);
  
  // Set new gammaTable:
  PsychGetCGDisplayIDFromScreenNumber(&cgDisplayID, screenNumber);
  XF86VidModeSetGammaRamp(cgDisplayID, PsychGetXScreenIdForScreen(screenNumber), 256, (unsigned short*) RTable, (unsigned short*) GTable, (unsigned short*) BTable);
#endif

  // Return "success":
  return(1);
}

// PsychGetDisplayBeamPosition() contains the implementation of display beamposition queries.
// It requires both, a cgDisplayID handle, and a logical screenNumber and uses one of both for
// deciding which display pipe to query, whatever of both is more efficient or suitable for the
// host platform -- This is ugly, but neccessary, because the mapping with only one of these
// specifiers would be either ambigous (wrong results!) or usage would be inefficient and slow
// (bad for such a time critical low level call!). On some systems it may even ignore the arguments,
// because it's not capable of querying different pipes - ie., it will always query a hard-coded pipe.
//
int PsychGetDisplayBeamPosition(CGDirectDisplayID cgDisplayId, int screenNumber)
{

	// Beamposition queries aren't supported by the X11 graphics system.
	// However, for gfx-hardware where we have reliable register specs, we
	// can do it ourselves, bypassing the X server.
	
	// On systems that we can't handle, we return -1 as an indicator
	// to high-level routines that we don't know the rasterbeam position.
	double tdeadline, tnow;
	int vblbias, vbltotal;
	int beampos = -1;

	// Get beamposition from low-level driver code:
	if (PsychOSIsKernelDriverAvailable(screenNumber) && displayBeampositionHealthy[screenNumber]) {
		// Is application of the beamposition workaround requested by high-level code?
		// Or is this a NVidia GPU? In the latter case we always use the workaround,
		// because many NVidia GPU's (especially pre NV-50 hardware) need this in many
		// setups. It helps if needed, and doesn't hurt if not needed - burns at most
		// 25 insignificant microseconds of time.
		if ((PsychPrefStateGet_ConserveVRAM() & kPsychUseBeampositionQueryWorkaround) ||
		    (fDeviceType == kPsychGeForce)) {
			// Yes: Avoid queries that return zero -- If query result is zero, retry
			// until it becomes non-zero: Some hardware may needs this to resolve...
			// We use a timeout of 100 msecs though to prevent hangs if we try to
			// query a disabled crtc's scanout position or similar bad things happen...
			PsychGetPrecisionTimerSeconds(&tdeadline);
			tdeadline += 0.1;
			while (0 == (beampos = PsychOSKDGetBeamposition(screenNumber))) {
				PsychGetPrecisionTimerSeconds(&tnow);
				if (tnow > tdeadline) {
					// Trouble: Hanging here for more than 100 msecs?
					// This display head is dead. Output a info to the user
					// and disable it for further beamposition queries.
					displayBeampositionHealthy[screenNumber] = FALSE;
					beampos = -1;

					if (PsychPrefStateGet_Verbosity() > 1) {
						printf("PTB-WARNING: Hang in beamposition query detected! Seems my mapping of screen numbers to GPU's and display outputs is wrong?\n");
						printf("PTB-WARNING: In a single GPU system you can resolve this by plugging in your monitors in a different order, changing the\n");
						printf("PTB-WARNING: display arrangement in the control panel, or using the Screen('Preference', 'ScreenToHead', screenId [, newHeadId]);\n");
						printf("PTB-WARNING: command at the top of your scripts to set the mapping manually.\n");
						printf("PTB-WARNING: \n");
						printf("PTB-WARNING: I am not yet able to handle multi-GPU systems reliably at all. If you have such a system it may work if\n");
						printf("PTB-WARNING: you plug your monitor(s) into one of the other GPU's output connectors, trying different combinations.\n");
						printf("PTB-WARNING: Or you simply live without high precision stimulus onset timestamping for now. Or you use the free and open-source\n");
						printf("PTB-WARNING: graphics drivers (intel, radeon, or nouveau) instead of the proprietary Catalyst or NVidia binary drivers.\n");
						printf("PTB-WARNING: I've disabled high precision timestamping for this screen for the remainder of the session.\n\n");
						fflush(NULL);
					}

					break;
				}
			}
		} else {
			// Read final beampos:
			beampos = PsychOSKDGetBeamposition(screenNumber);
		}
	}
	
	// Return failure, if indicated:
	if (beampos == -1) return(-1);
	
	// Apply corrective offsets if any (i.e., if non-zero):
	// Note: In case of Radeon's, these are zero, because the code above already has applied proper corrections.
	PsychGetBeamposCorrection(screenNumber, &vblbias, &vbltotal);
	beampos = beampos - vblbias;
	if (beampos < 0) beampos = vbltotal + beampos;
	
	// Return our result or non-result:
	return(beampos);
}

// Try to attach to kernel level ptb support driver and setup everything, if it works:
void InitPsychtoolboxKernelDriverInterface(void)
{
	// This is currently a no-op on Linux, as most low-level stuff is done via mmapped() MMIO access...
	return;
}

// Try to detach to kernel level ptb support driver and tear down everything:
void PsychOSShutdownPsychtoolboxKernelDriverInterface(void)
{
	if (numKernelDrivers > 0) {
		// Nothing to do yet...
	}

	// Ok, whatever happened, we're detached (for good or bad):
	numKernelDrivers = 0;

	return;
}

psych_bool PsychOSIsKernelDriverAvailable(int screenId)
{
	// Currently our "kernel driver" is available if MMIO mem could be mapped:
	// A real driver would indicate its presence via numKernelDrivers > 0 (see init/teardown code just above this routine):
	return((gfx_cntl_mem) ? TRUE : FALSE);
}

int PsychOSCheckKDAvailable(int screenId, unsigned int * status)
{
	// This doesn't make much sense on Linux yet. 'connect' should be something like a handle
	// to a kernel driver connection, e.g., the filedescriptor fd of the devicefile for ioctl()s
	// but we don't have such a thing yet.  Could be also a pointer to a little struct with all
	// relevant info...
	// Currently we do a dummy assignment...
	int connect = PsychScreenToHead(screenId);

	if ((numKernelDrivers<=0) && (gfx_cntl_mem == NULL)) {
		if (status) *status = ENODEV;
		return(0);
	}
	
	if (connect == 0xff) {
		if (status) *status = ENODEV;
		if (PsychPrefStateGet_Verbosity() > 6) printf("PTB-DEBUGINFO: Could not access kernel driver connection for screenId %i - No such connection.\n", screenId);
		return(0);
	}

	if (status) *status = 0;

	// Force this to '1', so the truth value is non-zero aka TRUE.
	connect = 1;
	return(connect);
}

unsigned int PsychOSKDReadRegister(int screenId, unsigned int offset, unsigned int* status)
{
	// Check availability of connection:
	int connect;
	if (!(connect = PsychOSCheckKDAvailable(screenId, status))) {
		return(0xffffffff);
		if (status) *status = ENODEV;
	}
	
	if (status) *status = 0;

	// Return readback register value:
	return(ReadRegister(offset));
}

unsigned int PsychOSKDWriteRegister(int screenId, unsigned int offset, unsigned int value, unsigned int* status)
{
	// Check availability of connection:
	int connect;
	if (!(connect = PsychOSCheckKDAvailable(screenId, status))) {
		return(0xffffffff);
		if (status) *status = ENODEV;
	}

	if (status) *status = 0;

	// Write the register:
	WriteRegister(offset, value);
	
	// Return success:
	return(0);
}

// Synchronize display screens video refresh cycle of DCE-4 (and later) GPU's, aka Evergreen. See PsychSynchronizeDisplayScreens() for help and details...
static PsychError PsychOSSynchronizeDisplayScreensDCE4(int *numScreens, int* screenIds, int* residuals, unsigned int syncMethod, double syncTimeOut, int allowedResidual)
{
	int								screenId = 0;
	double							abortTimeOut, now;
	int								residual;
	unsigned int					i;
	unsigned int					old_crtc_master_enable = 0;
	
	// Check availability of connection:
	int								connect;
	unsigned int					status;

	// No support for other methods than fast hard sync:
	if (syncMethod > 1) {
		if (PsychPrefStateGet_Verbosity() > 1) printf("PTB-WARNING: Could not execute display resync operation with requested non hard sync method. Not supported for this setup and settings.\n"); 
		return(PsychError_unimplemented);
	}
	
	// The current implementation only supports syncing all heads of a single card
	if (*numScreens <= 0) {
		// Resync all displays requested: Choose screenID zero for connect handle:
		screenId = 0;
	}
	else {
		// Resync of specific display requested: We only support resync of all heads of a single multi-head card,
		// therefore just choose the screenId of the passed master-screen for resync handle:
		screenId = screenIds[0];
	}
	
	if (!(connect = PsychOSCheckKDAvailable(screenId, &status))) {
		if (PsychPrefStateGet_Verbosity() > 1) printf("PTB-WARNING: Could not execute display resync operation for master screenId %i. Not supported for this setup and settings.\n", screenId); 
		return(PsychError_unimplemented);
	}
	
	if (fDeviceType != kPsychRadeon) {
		printf("PTB-INFO: PsychOSSynchronizeDisplayScreens(): This function is not supported on non-ATI/AMD GPU's! Aborted.\n");
		return(PsychError_unimplemented);
	}

	// Setup deadline for abortion or repeated retries:
	PsychGetAdjustedPrecisionTimerSeconds(&abortTimeOut);
	abortTimeOut+=syncTimeOut;
	residual = INT_MAX;
	
	// Repeat until timeout or good enough result:
	do {
		// If this isn't the first try, wait 0.5 secs before retry:
		if (residual != INT_MAX) PsychWaitIntervalSeconds(0.5);
		
		residual = INT_MAX;
		
		if (PsychPrefStateGet_Verbosity() > 3) printf("PTB-INFO: PsychOSSynchronizeDisplayScreens(): About to resynchronize all DCE-4 display heads by use of a 1 second CRTC stop->start cycle:\n");

		if (PsychPrefStateGet_Verbosity() > 3) {
			printf("Trying to stop and reset all display heads by disabling them one by one.\n");
			printf("Will wait individually for each head to reach its defined resting position.\n");
		}
		
		// Detect enabled heads:
		old_crtc_master_enable = 0;
		for (i = 0; i < 6; i++) {
			// Bit 16 "CRTC_CURRENT_MASTER_EN_STATE" allows read-only polling
			// of current activation state of crtc:
			if (ReadRegister(EVERGREEN_CRTC_CONTROL + crtcoff[i]) & (0x1 << 16)) old_crtc_master_enable |= (0x1 << i);
		}

		// Shut down heads, one after each other, wait for each one to settle at its defined resting position:
		for (i = 0; i < 6; i++) {
			if (PsychPrefStateGet_Verbosity() > 3) printf("Head %ld ...  ", i);
			if (old_crtc_master_enable & (0x1 << i)) {		
				if (PsychPrefStateGet_Verbosity() > 3) printf("active -> Shutdown. ");

				// Shut down this CRTC by clearing its master enable bit (bit 0):
				WriteRegister(EVERGREEN_CRTC_CONTROL + crtcoff[i], ReadRegister(EVERGREEN_CRTC_CONTROL + crtcoff[i]) & ~(0x1 << 0));
				
				// Wait 50 msecs, so CRTC has enough time to settle and disable at its
				// programmed resting position:
				PsychWaitIntervalSeconds(0.050);
				
				// Double check - Poll until crtc is offline:
				while(ReadRegister(EVERGREEN_CRTC_CONTROL + crtcoff[i]) & (0x1 << 16));
				if (PsychPrefStateGet_Verbosity() > 3) printf("-> Offline.\n");
			}
			else {
				if (PsychPrefStateGet_Verbosity() > 3) printf("already offline.\n");
			}
		}
		
		// Need realtime priority for following synchronized start to minimize delays:
		PsychRealtimePriority(true);

		// Sleep for 1 second: This is a blocking call, ie. the thread goes to sleep and may wakeup a bit later:
		PsychWaitIntervalSeconds(1);
		
		// Reenable all now disabled, but previously enabled display heads.
		// This must be a tight loop, as every microsecond counts for a good sync...
		for (i = 0; i < 6; i++) {
			if (old_crtc_master_enable & (0x1 << i)) {		
				// Restart this CRTC by setting its master enable bit (bit 0):
				WriteRegister(EVERGREEN_CRTC_CONTROL + crtcoff[i], ReadRegister(EVERGREEN_CRTC_CONTROL + crtcoff[i]) | (0x1 << 0));
			}
		}
		
		// Done with realtime bits:
		PsychRealtimePriority(false);

		// We don't have meaningful residual info. Just assume we succeeded:
		residual = 0;
		if (PsychPrefStateGet_Verbosity() > 2) printf("PTB-INFO: Graphics display heads hopefully resynchronized.\n");
		
		// Timestamp:
		PsychGetAdjustedPrecisionTimerSeconds(&now);
	} while ((now < abortTimeOut) && (abs(residual) > allowedResidual));
	
	// Return residual value if wanted:
	if (residuals) { 
		residuals[0] = residual;
	}
	
	if (abs(residual) > allowedResidual) {
		if (PsychPrefStateGet_Verbosity() > 1) printf("PTB-WARNING: Failed to synchronize heads down to the allowable residual of +/- %i scanlines. Final residual %i lines.\n", allowedResidual, residual);
	}
	
	// TODO: Error handling not really worked out...
	if (residual == INT_MAX) return(PsychError_system);
	
	// Done.
	return(PsychError_none);
}

// Helper function for PsychOSSynchronizeDisplayScreens().
static unsigned int GetBeamPosition(int headId)
{
	  return((unsigned int) ReadRegister((headId == 0) ? RADEON_D1CRTC_STATUS_POSITION : RADEON_D2CRTC_STATUS_POSITION) & RADEON_VBEAMPOSITION_BITMASK);
}

// Synchronize display screens video refresh cycle. See PsychSynchronizeDisplayScreens() for help and details...
PsychError PsychOSSynchronizeDisplayScreens(int *numScreens, int* screenIds, int* residuals, unsigned int syncMethod, double syncTimeOut, int allowedResidual)
{
	int								screenId = 0;
	double							abortTimeOut, now;
	int								residual;
	int								deltabeampos;
	unsigned int					beampos0, beampos1, i;
	unsigned int					old_crtc_master_enable = 0;
	unsigned int					new_crtc_master_enable = 0;
	
	// Check availability of connection:
	int								connect;
	unsigned int					status;
	
	// No support for other methods than fast hard sync:
	if (syncMethod > 1) {
		if (PsychPrefStateGet_Verbosity() > 1) printf("PTB-WARNING: Could not execute display resync operation with requested non hard sync method. Not supported for this setup and settings.\n"); 
		return(PsychError_unimplemented);
	}
	
	// The current implementation only supports syncing all heads of a single card
	if (*numScreens <= 0) {
		// Resync all displays requested: Choose screenID zero for connect handle:
		screenId = 0;
	}
	else {
		// Resync of specific display requested: We only support resync of all heads of a single multi-head card,
		// therefore just choose the screenId of the passed master-screen for resync handle:
		screenId = screenIds[0];
	}
	
	if (!(connect = PsychOSCheckKDAvailable(screenId, &status))) {
		if (PsychPrefStateGet_Verbosity() > 1) printf("PTB-WARNING: Could not execute display resync operation for master screenId %i. Not supported for this setup and settings.\n", screenId); 
		return(PsychError_unimplemented);
	}
	
	if (fDeviceType != kPsychRadeon) {
		printf("PTB-INFO: PsychOSSynchronizeDisplayScreens(): This function is not supported on non-ATI/AMD GPU's! Aborted.\n");
		return(PsychError_unimplemented);
	}

	// DCE-4 display engine of Evergreen or later?
	if (isDCE4(screenId) || isDCE5(screenId)) {
		// Yes. Use DCE-4 specific sync routine:
		return(PsychOSSynchronizeDisplayScreensDCE4(numScreens, screenIds, residuals, syncMethod, syncTimeOut, allowedResidual));
	}
	
	// Setup deadline for abortion or repeated retries:
	PsychGetAdjustedPrecisionTimerSeconds(&abortTimeOut);
	abortTimeOut+=syncTimeOut;
	residual = INT_MAX;
	
	// Repeat until timeout or good enough result:
	do {
		// If this isn't the first try, wait 0.5 secs before retry:
		if (residual != INT_MAX) PsychWaitIntervalSeconds(0.5);
		
		residual = INT_MAX;
		
		if (PsychPrefStateGet_Verbosity() > 3) printf("PTB-INFO: PsychOSSynchronizeDisplayScreens(): About to resynchronize all display heads by use of a 1 second CRTC stop->start cycle:\n");
		
		// A little pretest...
		if (PsychPrefStateGet_Verbosity() > 3) ("Pretest...\n");
		for (i = 0; i < 10; i++) { 
			beampos0 = GetBeamPosition(0);
			beampos1 = GetBeamPosition(1);
			if (PsychPrefStateGet_Verbosity() > 3) printf("Sample %ld: Beampositions are %ld vs. %ld . Offset %ld\n", i, beampos0, beampos1, (int) beampos1 - (int) beampos0);
		}
		
		// Query the CRTC scan-converter master enable state: Bit 0 (value 0x1) controls Pipeline 1,
		// whereas Bit 1(value 0x2) controls Pipeline 2:
		old_crtc_master_enable = ReadRegister(RADEON_DC_CRTC_MASTER_ENABLE);
		if (PsychPrefStateGet_Verbosity() > 3) {
			printf("Current CRTC Master enable state is %ld . Trying to stop and reset all display heads.\n", old_crtc_master_enable);
			printf("Will wait individually for each head to get close to scanline 0, then disable it.\n");
		}
		
		// Shut down heads, one after each other, each one at the start of a new refresh cycle:
		for (i = 0; i <= 1; i++) { 
			// Wait for head i to start a new display cycle (scanline 0), then shut it down - well if it is active at all:
			if (PsychPrefStateGet_Verbosity() > 3) printf("Head %ld ...  ", i);
			if (old_crtc_master_enable & (0x1 << i)) {		
				if (PsychPrefStateGet_Verbosity() > 3) printf("active -> Shutdown. ");
				// Wait for beam going above scanline 240: We choose 240, because even at the lowest conceivable
				// useful display resolution of 640 x 480, 240 will be in the middle of the frame, aka far away
				// from both, the start and the end of a frame:
				while (GetBeamPosition(i) <= 240);
				
				// Beam is heading for the end of the frame + VBL area. Wait for wrap-around, ie. until
				// reaching a scanline value smaller than 100 --> Until it wraps back to zero or at least
				// a value close to zero:
				while (GetBeamPosition(i) > 240);
				
				// Start of new refresh interval! Shut down this heads CRTC!
				// We do so by clearing enable bit for this head:
				WriteRegister(RADEON_DC_CRTC_MASTER_ENABLE, ReadRegister(RADEON_DC_CRTC_MASTER_ENABLE) & ~(0x1 << i));
				if (PsychPrefStateGet_Verbosity() > 3) printf("New state is %ld.\n", ReadRegister(RADEON_DC_CRTC_MASTER_ENABLE));
				
				// Head should be down, close to scanline 0.
				PsychWaitIntervalSeconds(0.050);
			}
			else {
				if (PsychPrefStateGet_Verbosity() > 3) printf("already offline.\n");
			}
		}
		
		// All display heads should be disabled now.
		PsychWaitIntervalSeconds(0.100);
		
		// Query current beamposition and check state:
		beampos0 = GetBeamPosition(0);
		beampos1 = GetBeamPosition(1);
		
		new_crtc_master_enable = ReadRegister(RADEON_DC_CRTC_MASTER_ENABLE);
		
		if (new_crtc_master_enable == 0) {
			if (PsychPrefStateGet_Verbosity() > 3) printf("CRTC's down (state %ld): Beampositions are [0]=%ld and [1]=%ld. Synchronized restart in 1 second...\n", new_crtc_master_enable, beampos0, beampos1);
		}
		else {
			if (PsychPrefStateGet_Verbosity() > 3) printf("CRTC's shutdown failed!! (state %ld): Beamposition are [0]=%ld and [1]=%ld. Will try to restart in 1 second...\n", new_crtc_master_enable, beampos0, beampos1);
		}
		
		// Sleep for 1 second: This is a blocking call, ie. the thread goes to sleep and may wakeup a bit later:
		PsychWaitIntervalSeconds(1);
		
		// Reset all display heads enable state to original setting:
		WriteRegister(RADEON_DC_CRTC_MASTER_ENABLE, old_crtc_master_enable);
		
		// Query position and state after restart:
		beampos0 = GetBeamPosition(0);
		beampos1 = GetBeamPosition(1);
		new_crtc_master_enable = ReadRegister(RADEON_DC_CRTC_MASTER_ENABLE);
		if (new_crtc_master_enable == old_crtc_master_enable) {
			if (PsychPrefStateGet_Verbosity() > 3) printf("CRTC's restarted in sync: Master enable state is %ld. Beampositions after restart: [0]=%ld and [1]=%ld.\n", new_crtc_master_enable, beampos0, beampos1);
		}
		else {
			if (PsychPrefStateGet_Verbosity() > 3) printf("CRTC's restart FAILED!!: Master enable state is %ld. Beampositions: [0]=%ld and [1]=%ld.\n", new_crtc_master_enable, beampos0, beampos1);
		}
		
		deltabeampos = (int) beampos1 - (int) beampos0;
		if (PsychPrefStateGet_Verbosity() > 3) printf("Residual beam offset after display sync: %ld.\n\n", deltabeampos);
		
		// A little posttest...
		if (PsychPrefStateGet_Verbosity() > 3) printf("Posttest...\n");
		for (i = 0; i < 10; i++) { 
			beampos0 = GetBeamPosition(0);
			beampos1 = GetBeamPosition(1);
			if (PsychPrefStateGet_Verbosity() > 3) printf("Sample %ld: Beampositions are %ld vs. %ld . Offset %ld\n", i, beampos0, beampos1, (int) beampos1 - (int) beampos0);
		}
		
		if (PsychPrefStateGet_Verbosity() > 3) printf("Display head resync operation finished.\n\n");
		
		// Assign residual for this iteration:
		residual = deltabeampos;
		
		if (PsychPrefStateGet_Verbosity() > 2) printf("PTB-INFO: Graphics display heads resynchronized. Residual vertical beamposition error is %ld scanlines.\n", residual);
		
		// Timestamp:
		PsychGetAdjustedPrecisionTimerSeconds(&now);
	} while ((now < abortTimeOut) && (abs(residual) > allowedResidual));
	
	// Return residual value if wanted:
	if (residuals) { 
		residuals[0] = residual;
	}
	
	if (abs(residual) > allowedResidual) {
		if (PsychPrefStateGet_Verbosity() > 1) printf("PTB-WARNING: Failed to synchronize heads down to the allowable residual of +/- %i scanlines. Final residual %i lines.\n", allowedResidual, residual);
	}
	
	// TODO: Error handling not really worked out...
	if (residual == INT_MAX) return(PsychError_system);
	
	// Done.
	return(PsychError_none);
}

int PsychOSKDGetBeamposition(int screenId)
{
	int beampos = -1;
	int headId  = PsychScreenToHead(screenId);

	// MMIO registers mapped?
	if (gfx_cntl_mem) {
		// Query code for ATI/AMD Radeon/FireGL/FirePro:
		if (fDeviceType == kPsychRadeon) {
			if (isDCE4(screenId) || isDCE5(screenId)) {
				// DCE-4 display engine (CEDAR and later afaik): Up to six crtc's.
                if (headId > DCE4_MAXHEADID) {
                    printf("PTB-ERROR: PsychOSKDGetBeamposition: Invalid headId %i provided! Must be between 0-5!\n", headId);
                    return(beampos);
				}
                
				// Read raw beampostion from GPU:
				beampos = (int) (ReadRegister(EVERGREEN_CRTC_STATUS_POSITION + crtcoff[headId]) & RADEON_VBEAMPOSITION_BITMASK);
				
				// Query end-offset of VBLANK interval of this GPU and correct for it:
				beampos = beampos - (int) ((ReadRegister(EVERGREEN_CRTC_V_BLANK_START_END + crtcoff[headId]) >> 16) & RADEON_VBEAMPOSITION_BITMASK);
				
				// Correction for in-VBLANK range:
				if (beampos < 0) beampos = ((int) ReadRegister(EVERGREEN_CRTC_V_TOTAL + crtcoff[headId])) + beampos;
				
			} else {
				// AVIVO display engine (R300 - R600 afaik): At most two display heads for dual-head gpu's.
				
				// Read raw beampostion from GPU:
				beampos = (int) (ReadRegister((headId == 0) ? RADEON_D1CRTC_STATUS_POSITION : RADEON_D2CRTC_STATUS_POSITION) & RADEON_VBEAMPOSITION_BITMASK);
				
				// Query end-offset of VBLANK interval of this GPU and correct for it:
				beampos = beampos - (int) ((ReadRegister((headId == 0) ? AVIVO_D1CRTC_V_BLANK_START_END : AVIVO_D2CRTC_V_BLANK_START_END) >> 16) & RADEON_VBEAMPOSITION_BITMASK);
				
				// Correction for in-VBLANK range:
				if (beampos < 0) beampos = ((int) ReadRegister((headId == 0) ? AVIVO_D1CRTC_V_TOTAL : AVIVO_D2CRTC_V_TOTAL)) + beampos;
			}
		}
		
		// Query code for NVidia GeForce/Quadro:
		if (fDeviceType == kPsychGeForce) {
			// Pre NV-50 GPU? [Anything before GeForce-8 series]
			if (fCardType < 0x50) {
				// Pre NV-50, e.g., RivaTNT-1/2 and all GeForce 256/2/3/4/5/FX/6/7:
				
				// Lower 12 bits are vertical scanout position (scanline), bit 16 is "in vblank" indicator.
				// Offset between crtc's is 0x2000, we're only interested in scanline, not "in vblank" status:
				// beampos = (int) (ReadRegister((headId == 0) ? 0x600808 : 0x600808 + 0x2000) & 0xFFF);

				// NV-47: Lower 16 bits are vertical scanout position (scanline), upper 16 bits are horizontal
				// scanout position. Offset between crtc's is 0x2000. We only use the lower 16 bits and
				// ignore horizontal scanout position for now:
				beampos = (int) (ReadRegister((headId == 0) ? 0x600868 : 0x600868 + 0x2000) & 0xFFFF);
			} else {
				// NV-50 (GeForce-8) and later:
				
				// Lower 16 bits are vertical scanout position (scanline), upper 16 bits are vblank counter.
				// Offset between crtc's is 0x800, we're only interested in scanline, not vblank counter:
				beampos = (int) (ReadRegister((headId == 0) ? 0x616340 : 0x616340 + 0x800) & 0xFFFF);
			}
		}

		// Query code for Intel IGP's:
		if (fDeviceType == kPsychIntelIGP) {
				beampos = (int) (ReadRegister((headId == 0) ? 0x70000 : 0x70000 + 0x1000) & 0x1FFF);
		}

		// Safety measure: Cap to zero if something went wrong -> This will trigger proper high level error handling in PTB:
		if (beampos < 0) beampos = -1;
	}

	return(beampos);
}

// Try to change hardware dither mode on GPU:
void PsychOSKDSetDitherMode(int screenId, unsigned int ditherOn)
{
    static unsigned int oldDither[(DCE4_MAXHEADID + 1)] = { 0, 0, 0, 0, 0, 0 };
    unsigned int reg;
	int headId  = (screenId >= 0) ? PsychScreenToHead(screenId) : -screenId;
    
    // MMIO registers mapped?
	if (!gfx_cntl_mem) return;

    // AMD/ATI Radeon, FireGL or FirePro GPU?
	if (fDeviceType == kPsychRadeon) {
        if (PsychPrefStateGet_Verbosity() > 2) printf("PTB-INFO: SetDitherMode: Trying to %s digital display dithering on display head %d.\n", (ditherOn) ? "enable" : "disable", headId);

        // Map headId to proper hardware control register offset:
		if (isDCE4(screenId) || isDCE5(screenId)) {
			// DCE-4 display engine (CEDAR and later afaik): Up to six crtc's. Map to proper
            // register offset for this headId:
            if (headId > DCE4_MAXHEADID) {
                // Invalid head - bail:
                if (PsychPrefStateGet_Verbosity() > 0) printf("SetDitherMode: ERROR! Invalid headId %d provided. Must be between 0 and 5. Aborted.\n", headId);
                return;
            }

            // Map to dither format control register for head 'headId':
            reg = EVERGREEN_FMT_BIT_DEPTH_CONTROL + crtcoff[headId];
		} else {
			// AVIVO display engine (R300 - R600 afaik): At most two display heads for dual-head gpu's.
            if (headId > 1) {
                if (PsychPrefStateGet_Verbosity() > 2) printf("PTB-INFO: SetDitherMode: INFO! Special headId %d outside valid dualhead range 0-1 provided. Will control LVDS dithering.\n", headId);
                headId = 0;
            }
            else {
                if (PsychPrefStateGet_Verbosity() > 2) printf("PTB-INFO: SetDitherMode: INFO! headId %d in valid dualhead range 0-1 provided. Will control TMDS (DVI et al.) dithering.\n", headId);
                headId = 1;
            }
            
            // On AVIVO we can't control dithering per display head. Instead there's one global switch
            // for LVDS connected displays (LVTMA) aka internal flat panels, e.g., of Laptops, and
            // on global switch for "all things DVI-D", aka TMDSA:
            reg = (headId == 0) ? RADEON_LVTMA_BIT_DEPTH_CONTROL : RADEON_TMDSA_BIT_DEPTH_CONTROL;
		}

        // Perform actual enable/disable/reconfigure sequence for target encoder/head:

        // Enable dithering?
        if (ditherOn) {
            // Reenable dithering with old, previously stored settings, if it is disabled:
            
            // Dithering currently off (all zeros)?
            if (ReadRegister(reg) == 0) {
                // Dithering is currently off. Do we know the old setting from a previous
                // disable?
                if (oldDither[headId] > 0) {
                    // Yes: Restore old "factory settings":
                    if (PsychPrefStateGet_Verbosity() > 2) printf("PTB-INFO: SetDitherMode: Dithering previously disabled by us. Reenabling with old control setting %x.\n", oldDither[headId]);
                    WriteRegister(reg, oldDither[headId]);
                }
                else {
                    // No: Dithering was disabled all the time, so we don't know the
                    // OS defaults. Use the numeric value of 'ditherOn' itself:
                    if (PsychPrefStateGet_Verbosity() > 2) printf("PTB-INFO: SetDitherMode: Dithering off. Enabling with userspace provided setting %x. Cross your fingers!\n", ditherOn);
                    WriteRegister(reg, ditherOn);
                }
            }
            else {
                if (PsychPrefStateGet_Verbosity() > 2) printf("PTB-INFO: SetDitherMode: Dithering already enabled with current control value %x. Skipped.\n", ReadRegister(reg));
            }
        }
        else {
            // Disable all dithering if it is enabled: Clearing the register to all zero bits does this.
            if (ReadRegister(reg) > 0) {
                oldDither[headId] = ReadRegister(reg);
                if (PsychPrefStateGet_Verbosity() > 2) printf("PTB-INFO: SetDitherMode: Current dither setting before our dither disable on head %d is %x. Disabling.\n", headId, oldDither[headId]);
                WriteRegister(reg, 0x0);
            }
            else {
                if (PsychPrefStateGet_Verbosity() > 2) printf("PTB-INFO: SetDitherMode: Dithering already disabled. Skipped.\n");
            }
        }
        
        // End of Radeon et al. support code.
	}
    else {
        // Other unsupported GPU:
        if (PsychPrefStateGet_Verbosity() > 2) printf("PTB-INFO: SetDitherMode: Tried to call me on a non ATI/AMD GPU. Unsupported.\n");
    }
    
	return;
}

// Query if LUT for given headId is all-zero: 0 = Something else, 1 = Zero-LUT, 2 = It's an identity LUT,
// 3 = Not-quite-identity mapping, 0xffffffff = don't know.
unsigned int PsychOSKDGetLUTState(int screenId, unsigned int headId, unsigned int debug)
{
    unsigned int i, v, r, m, bo, wo, offset, reg;
    unsigned int isZero = 1;
    unsigned int isIdentity = 1;
    
    // AMD GPU's:
	if (fDeviceType == kPsychRadeon) {
        if (PsychPrefStateGet_Verbosity() > 3) printf("PsychOSKDGetLUTState(): Checking LUT and bias values on GPU for headId %d.\n", headId);

        if (isDCE4(screenId) || isDCE5(screenId)) {
            // DCE-4.0 and later: Up to (so far) six display heads:
            if (headId > DCE4_MAXHEADID) {
                // Invalid head - bail:
                if (PsychPrefStateGet_Verbosity() > 2) printf("PsychOSKDGetLUTState: ERROR! Invalid headId %d provided. Must be between 0 and 5. Aborted.\n", headId);
                return(0xffffffff);
            }

            offset = crtcoff[headId];
            WriteRegister(EVERGREEN_DC_LUT_RW_MODE + offset, 0);
            WriteRegister(EVERGREEN_DC_LUT_RW_INDEX + offset, 0);
            reg = EVERGREEN_DC_LUT_30_COLOR + offset;
            
            // Find out if there are non-zero black offsets:
            bo = 0x0;
            bo|= ReadRegister(EVERGREEN_DC_LUT_BLACK_OFFSET_BLUE + offset);
            bo|= ReadRegister(EVERGREEN_DC_LUT_BLACK_OFFSET_GREEN + offset);
            bo|= ReadRegister(EVERGREEN_DC_LUT_BLACK_OFFSET_RED + offset);
            
            // Find out if there are non-0xffff white offsets:
            wo = 0x0;
            wo|= 0xffff - ReadRegister(EVERGREEN_DC_LUT_WHITE_OFFSET_BLUE + offset);
            wo|= 0xffff - ReadRegister(EVERGREEN_DC_LUT_WHITE_OFFSET_GREEN + offset);
            wo|= 0xffff - ReadRegister(EVERGREEN_DC_LUT_WHITE_OFFSET_RED + offset);
        }
        else {
            // AVIVO: Dualhead.
            offset = (headId > 0) ? 0x800 : 0x0;
            WriteRegister(AVIVO_DC_LUT_RW_SELECT, headId & 0x1);
            WriteRegister(AVIVO_DC_LUT_RW_MODE, 0);
            WriteRegister(AVIVO_DC_LUT_RW_INDEX, 0);
            reg = AVIVO_DC_LUT_30_COLOR;

            // Find out if there are non-zero black offsets:
            bo = 0x0;
            bo|= ReadRegister(AVIVO_DC_LUTA_BLACK_OFFSET_BLUE + offset);
            bo|= ReadRegister(AVIVO_DC_LUTA_BLACK_OFFSET_GREEN + offset);
            bo|= ReadRegister(AVIVO_DC_LUTA_BLACK_OFFSET_RED + offset);
            
            // Find out if there are non-0xffff white offsets:
            wo = 0x0;
            wo|= 0xffff - ReadRegister(AVIVO_DC_LUTA_WHITE_OFFSET_BLUE + offset);
            wo|= 0xffff - ReadRegister(AVIVO_DC_LUTA_WHITE_OFFSET_GREEN + offset);
            wo|= 0xffff - ReadRegister(AVIVO_DC_LUTA_WHITE_OFFSET_RED + offset);
        }

        if (debug) if (PsychPrefStateGet_Verbosity() > 3) printf("PsychOSKDOffsets: Black %d : White %d.\n", bo, wo);
        
        for (i = 0; i < 256; i++) {            
            // Read 32 bit value of this slot, mask out upper 2 bits,
            // so the least significant 30 bits are left, as these
            // contain the 3 * 10 bits for the 10 bit R,G,B channels:
            v = ReadRegister(reg) & (0xffffffff >> 2);
            
            // All zero as they should be for a all-zero LUT?
            if (v > 0) isZero = 0;
            
            // Compare with expected value in slot i for a perfect 10 bit identity LUT
            // intended for a 8 bit output encoder, i.e., 2 least significant bits
            // zero to avoid dithering and similar stuff:
            r = i << 2;
            m = (r << 20) | (r << 10) | (r << 0); 
            
            // Mismatch? Not a perfect identity LUT:
            if (v != m) isIdentity = 0;

            if (PsychPrefStateGet_Verbosity() > 4) {
                printf("%d:%d,%d,%d\n", i, (v >> 20) & 0x3ff, (v >> 10) & 0x3ff, (v >> 0) & 0x3ff);
            }
        }

        if (isZero) return(1);  // All zero LUT.

        if (isIdentity) {
            // If wo or bo is non-zero then it is not quite an identity
            // mapping, as the black and white offset are not neutral.
            // Return 3 in this case:
            if ((wo | bo) > 0) return(3);
            
            // Perfect identity LUT:
            return(2);
        }

        // Regular LUT:
        return(0);
	}

    // Unhandled:
    if (PsychPrefStateGet_Verbosity() > 3) printf("PsychOSKDGetLUTState(): This function is not supported on this GPU. Returning 0xffffffff.\n");
    return(0xffffffff);
}

// Load an identity LUT into display head 'headid': Return 1 on success, 0 on failure or if unsupported for this GPU:
unsigned int PsychOSKDLoadIdentityLUT(int screenId, unsigned int headId)
{
    unsigned int i, r, m, offset, reg;
    
    // AMD GPU's:
	if (fDeviceType == kPsychRadeon) {
        if (PsychPrefStateGet_Verbosity() > 3) printf("PsychOSKDLoadIdentityLUT(): Uploading identity LUT and bias values into GPU for headId %d.\n", headId);

        if (isDCE4(screenId) || isDCE5(screenId)) {
            // DCE-4.0 and later: Up to (so far) six display heads:
            if (headId > DCE4_MAXHEADID) {
                // Invalid head - bail:
                if (PsychPrefStateGet_Verbosity() > 3) printf("PsychOSKDLoadIdentityLUT: ERROR! Invalid headId %d provided. Must be between 0 and 5. Aborted.\n", headId);
                return(0);
            }

            offset = crtcoff[headId];
            reg = EVERGREEN_DC_LUT_30_COLOR + offset;
            
            WriteRegister(EVERGREEN_DC_LUT_CONTROL + offset, 0);

            if (isDCE5(screenId)) {
                WriteRegister(NI_INPUT_CSC_CONTROL + offset,
                              (NI_INPUT_CSC_GRPH_MODE(NI_INPUT_CSC_BYPASS) |
                               NI_INPUT_CSC_OVL_MODE(NI_INPUT_CSC_BYPASS)));
                WriteRegister(NI_PRESCALE_GRPH_CONTROL + offset,
                              NI_GRPH_PRESCALE_BYPASS);
                WriteRegister(NI_PRESCALE_OVL_CONTROL + offset,
                              NI_OVL_PRESCALE_BYPASS);
                WriteRegister(NI_INPUT_GAMMA_CONTROL + offset,
                              (NI_GRPH_INPUT_GAMMA_MODE(NI_INPUT_GAMMA_USE_LUT) |
                               NI_OVL_INPUT_GAMMA_MODE(NI_INPUT_GAMMA_USE_LUT)));
            }

            // Set zero black offsets:
            WriteRegister(EVERGREEN_DC_LUT_BLACK_OFFSET_BLUE  + offset, 0x0);
            WriteRegister(EVERGREEN_DC_LUT_BLACK_OFFSET_GREEN + offset, 0x0);
            WriteRegister(EVERGREEN_DC_LUT_BLACK_OFFSET_RED   + offset, 0x0);
            
            // Set 0xffff white offsets:
            WriteRegister(EVERGREEN_DC_LUT_WHITE_OFFSET_BLUE  + offset, 0xffff);
            WriteRegister(EVERGREEN_DC_LUT_WHITE_OFFSET_GREEN + offset, 0xffff);
            WriteRegister(EVERGREEN_DC_LUT_WHITE_OFFSET_RED   + offset, 0xffff);

            WriteRegister(EVERGREEN_DC_LUT_RW_MODE + offset, 0);
            WriteRegister(EVERGREEN_DC_LUT_WRITE_EN_MASK + offset, 0x00000007);

            WriteRegister(EVERGREEN_DC_LUT_RW_INDEX + offset, 0);

        }
        else {
            // AVIVO: Dualhead.
            offset = (headId > 0) ? 0x800 : 0x0;
            reg = AVIVO_DC_LUT_30_COLOR;

            WriteRegister(AVIVO_DC_LUTA_CONTROL + offset, 0);

            // Set zero black offsets:
            WriteRegister(AVIVO_DC_LUTA_BLACK_OFFSET_BLUE  + offset, 0x0);
            WriteRegister(AVIVO_DC_LUTA_BLACK_OFFSET_GREEN + offset, 0x0);
            WriteRegister(AVIVO_DC_LUTA_BLACK_OFFSET_RED   + offset, 0x0);
            
            // Set 0xffff white offsets:
            WriteRegister(AVIVO_DC_LUTA_WHITE_OFFSET_BLUE  + offset, 0xffff);
            WriteRegister(AVIVO_DC_LUTA_WHITE_OFFSET_GREEN + offset, 0xffff);
            WriteRegister(AVIVO_DC_LUTA_WHITE_OFFSET_RED   + offset, 0xffff);

            WriteRegister(AVIVO_DC_LUT_RW_SELECT, headId & 0x1);
            WriteRegister(AVIVO_DC_LUT_RW_MODE, 0);
            WriteRegister(AVIVO_DC_LUT_WRITE_EN_MASK, 0x0000003f);

            WriteRegister(AVIVO_DC_LUT_RW_INDEX, 0);
        }
        
        for (i = 0; i < 256; i++) {
            // Compute perfect value for slot i for a perfect 10 bit identity LUT
            // intended for a 8 bit output encoder, i.e., 2 least significant bits
            // zero to avoid dithering and similar stuff, the 8 most significant
            // bits for each 10 bit color channel linearly increasing one unit
            // per slot:
            r = i << 2;
            m = (r << 20) | (r << 10) | (r << 0); 

            // Write 32 bit value of this slot:
            WriteRegister(reg, m);
        }

        if (isDCE5(screenId)) {
            WriteRegister(NI_DEGAMMA_CONTROL + offset,
                          (NI_GRPH_DEGAMMA_MODE(NI_DEGAMMA_BYPASS) |
                           NI_OVL_DEGAMMA_MODE(NI_DEGAMMA_BYPASS) |
                           NI_ICON_DEGAMMA_MODE(NI_DEGAMMA_BYPASS) |
                           NI_CURSOR_DEGAMMA_MODE(NI_DEGAMMA_BYPASS)));
            WriteRegister(NI_GAMUT_REMAP_CONTROL + offset,
                          (NI_GRPH_GAMUT_REMAP_MODE(NI_GAMUT_REMAP_BYPASS) |
                           NI_OVL_GAMUT_REMAP_MODE(NI_GAMUT_REMAP_BYPASS)));
            WriteRegister(NI_REGAMMA_CONTROL + offset,
                          (NI_GRPH_REGAMMA_MODE(NI_REGAMMA_BYPASS) |
                           NI_OVL_REGAMMA_MODE(NI_REGAMMA_BYPASS)));
            WriteRegister(NI_OUTPUT_CSC_CONTROL + offset,
                          (NI_OUTPUT_CSC_GRPH_MODE(NI_OUTPUT_CSC_BYPASS) |
                           NI_OUTPUT_CSC_OVL_MODE(NI_OUTPUT_CSC_BYPASS)));
            /* XXX match this to the depth of the crtc fmt block, move to modeset? */
            WriteRegister(0x6940 + offset, 0);
        }
        
        // Done.
        return(1);
	}

    // Unhandled:
    if (PsychPrefStateGet_Verbosity() > 3) printf("PsychOSKDLoadIdentityLUT(): This function is not supported on this GPU. Returning 0.\n");
    return(0);
}
