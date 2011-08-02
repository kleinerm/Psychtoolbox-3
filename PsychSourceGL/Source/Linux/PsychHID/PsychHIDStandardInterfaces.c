/*
  PsychToolbox3/Source/Linux/PsychHID/PsychHIDStandardInterfaces.c
  
  PROJECTS: PsychHID only.
  
  PLATFORMS:  Linux.
  
  AUTHORS:
  mario.kleiner@tuebingen.mpg.de    mk

  HISTORY:
  27.07.2011     mk     Created.
  
  TO DO:

*/

#include "PsychHIDStandardInterfaces.h"

static Display *dpy = NULL;
static int xi_opcode, event, error;
static XIDeviceInfo *info = NULL;
static int ndevices = 0;
static int masterDevice = -1;
static XDevice* x_dev[PSYCH_HID_MAX_DEVICES];

static	double* psychHIDKbQueueFirstPress[PSYCH_HID_MAX_DEVICES];
static	double* psychHIDKbQueueFirstRelease[PSYCH_HID_MAX_DEVICES];
static	double* psychHIDKbQueueLastPress[PSYCH_HID_MAX_DEVICES];
static	double* psychHIDKbQueueLastRelease[PSYCH_HID_MAX_DEVICES];
static  psych_bool psychHIDKbQueueActive[PSYCH_HID_MAX_DEVICES];
static  psych_mutex KbQueueMutex;
static	psych_bool  KbQueueThreadTerminate;
static  psych_thread KbQueueThread;
static	XEvent KbQueue_xevent;

static XDevice* GetXDevice(int deviceIndex)
{
	if (deviceIndex < 0 || deviceIndex >= PSYCH_HID_MAX_DEVICES) PsychErrorExitMsg(PsychError_user, "Invalid deviceIndex specified. No such device!");
	if (x_dev[deviceIndex] == NULL) x_dev[deviceIndex] = XOpenDevice(dpy, (XID) info[deviceIndex].deviceid);
	return(x_dev[deviceIndex]);
}

void PsychHIDInitializeHIDStandardInterfaces(void)
{
	int major, minor;
	int rc, i;

	// Init x_dev array:
	for (i = 0; i < PSYCH_HID_MAX_DEVICES; i++) x_dev[i] = NULL;

	// Init keyboard queue arrays:
	memset(&psychHIDKbQueueFirstPress[0], 0, sizeof(psychHIDKbQueueFirstPress));
	memset(&psychHIDKbQueueFirstRelease[0], 0, sizeof(psychHIDKbQueueFirstRelease));
	memset(&psychHIDKbQueueLastPress[0], 0, sizeof(psychHIDKbQueueLastPress));
	memset(&psychHIDKbQueueLastRelease[0], 0, sizeof(psychHIDKbQueueLastRelease));
	memset(&psychHIDKbQueueActive[0], 0, sizeof(psychHIDKbQueueActive));

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

	// Open our own private X-Display connection for HID handling:
	dpy = XOpenDisplay(NULL);
	if (!dpy) {
		PsychErrorExitMsg(PsychError_system, "PsychHID: FATAL ERROR: Couldn't open default X11 display connection to X-Server! Game over!");
	}

	// XInputExtension supported? If so do basic init:
	if (!XQueryExtension(dpy, "XInputExtension", &xi_opcode, &event, &error)) {
		printf("PsychHID: ERROR: XINPUT extension unsupported!\n");
		goto out;
	}

	// XInput V2 supported?
	major = 2;
	minor = 0;
	rc = XIQueryVersion(dpy, &major, &minor);
	if (rc == BadRequest) {
		printf("PsychHID: ERROR: No XI2 support. Server supports version %d.%d only.\n", major, minor);
		goto out;
	} else if (rc != Success) {
		printf("PsychHID: ERROR: Internal Error! This is a bug in Xlib.\n");
		goto out;
	}

	// printf("PsychHID: INFO: XI2 supported. Server provides version %d.%d.\n", major, minor);

	// Enumerate all XI2 input devices:
	info = XIQueryDevice(dpy, XIAllDevices, &ndevices);

	masterDevice = -1;
	for(i = 0; i < ndevices; i++) {
		if (info[i].use == XIMasterPointer) {
			masterDevice = i;
			break;
		}
	}
	if (masterDevice == -1) printf("PsychHID: WARNING! No master keyboard/pointer found! This will end badly...\n");

	// Switch X-Server connection into synchronous mode: We need this to get
	// a higher timing precision.
	XSynchronize(dpy, TRUE);

	// Create keyboard queue mutex for later use:
	KbQueueThreadTerminate = FALSE;
	PsychInitMutex(&KbQueueMutex);

	return;

out:
	XCloseDisplay(dpy);
	dpy = NULL;
	PsychErrorExitMsg(PsychError_system, "PsychHID: FATAL ERROR: X Input extension version 2.0 or later not available! Game over!");	
}

void PsychHIDShutdownHIDStandardInterfaces(void)
{
	int i;

	// Close all devices registered in x_dev array:
	for (i = 0; i < PSYCH_HID_MAX_DEVICES; i++) {
		if (x_dev[i]) XCloseDevice(dpy, x_dev[i]);
		x_dev[i] = NULL;
	}

	// Release list of enumerated input devices:
	XIFreeDeviceInfo(info);
	info = NULL;

	// Release all keyboard queues:
	for (i = 0; i < PSYCH_HID_MAX_DEVICES; i++) {
		if (psychHIDKbQueueFirstPress[i]) {
			PsychHIDOSKbQueueRelease(i);
		}
	}

	// Release keyboard queue mutex:
	PsychDestroyMutex(&KbQueueMutex);
	KbQueueThreadTerminate = FALSE;

	// Close our dedicated x-display connection and we are done:
	XCloseDisplay(dpy);
	dpy = NULL;

	return;
}

PsychError PsychHIDEnumerateHIDInputDevices(int deviceClass)
{
    const char *deviceFieldNames[]={"usagePageValue", "usageValue", "usageName", "index", "transport", "vendorID", "productID", "version", 
                                    "manufacturer", "product", "serialNumber", "locationID", "interfaceID", "totalElements", "features", "inputs", 
                                    "outputs", "collections", "axes", "buttons", "hats", "sliders", "dials", "wheels"};
    int numDeviceStructElements, numDeviceStructFieldNames=24, deviceIndex;
    PsychGenericScriptType	*deviceStruct;
    XIDeviceInfo *dev;
    int i, j;
    int numKeys, numAxis;
    char *type = "";

    // Preparse: Count matching devices for deviceClass
    numDeviceStructElements = 0;
    for(i = 0; i < ndevices; i++) {
        dev = &info[i];
	if ((int) (dev->use) == deviceClass) numDeviceStructElements++;
    }

    // Alloc struct array of sufficient size:
    PsychAllocOutStructArray(1, FALSE, numDeviceStructElements, numDeviceStructFieldNames, deviceFieldNames, &deviceStruct);
    deviceIndex = 0;

    // Return info:
    for(i = 0; i < ndevices; i++) {
	// Check i'th device:
        dev = &info[i];

	// Skip if non matching class:
	if ((int) (dev->use) != deviceClass) continue;

        switch(dev->use) {
            case XIMasterPointer: type = "master pointer"; break;
            case XIMasterKeyboard: type = "master keyboard"; break;
            case XISlavePointer: type = "slave pointer"; break;
            case XISlaveKeyboard: type = "slave keyboard"; break;
            case XIFloatingSlave: type = "floating slave"; break;
        }

	// Usagepage is 1 for "Desktop usage page":
        PsychSetStructArrayDoubleElement("usagePageValue",	deviceIndex, 	(double) 1, deviceStruct);

	if (dev->use == XIMasterKeyboard || dev->use == XISlaveKeyboard) {
		// Usage 6 is for keyboard:
		PsychSetStructArrayDoubleElement("usageValue",	deviceIndex, (double) 6, deviceStruct);
	}
	else if (dev->use != XIFloatingSlave) {
		// Usage is 2 for mouse:
		PsychSetStructArrayDoubleElement("usageValue",	deviceIndex, (double) 2, deviceStruct);
	} else {
		// Assign meaningless Usage of zero for floating slave devices:
		PsychSetStructArrayDoubleElement("usageValue",	deviceIndex, (double) 0, deviceStruct);
	}

        PsychSetStructArrayStringElement("usageName",		deviceIndex, 	type, deviceStruct);
        PsychSetStructArrayDoubleElement("index",		deviceIndex, 	(double) i, deviceStruct);
        PsychSetStructArrayStringElement("transport",		deviceIndex, 	(dev->enabled) ? "enabled" : "disabled", deviceStruct);
        PsychSetStructArrayStringElement("product",		deviceIndex, 	dev->name, deviceStruct);
        PsychSetStructArrayDoubleElement("locationID",		deviceIndex, 	(double) dev->attachment, deviceStruct);
        PsychSetStructArrayDoubleElement("interfaceID",		deviceIndex, 	(double) dev->deviceid, deviceStruct);

        //PsychSetStructArrayDoubleElement("vendorID",		deviceIndex, 	(double)currentDevice->vendorID, 	deviceStruct);
        //PsychSetStructArrayDoubleElement("productID",		deviceIndex, 	(double)currentDevice->productID, 	deviceStruct);
        //PsychSetStructArrayDoubleElement("version",		deviceIndex, 	(double)currentDevice->version, 	deviceStruct);
        //PsychSetStructArrayStringElement("manufacturer",	deviceIndex, 	currentDevice->manufacturer, 		deviceStruct);
        //PsychSetStructArrayStringElement("serialNumber",	deviceIndex, 	currentDevice->serial, 			deviceStruct);

	numKeys = numAxis = 0;
	for (j = 0; j < dev->num_classes; j++) {
		if (dev->classes[j]->type == XIKeyClass) numKeys += (int) (((XIKeyClassInfo*) dev->classes[j])->num_keycodes);
		if (dev->classes[j]->type == XIButtonClass) numKeys += (int) (((XIButtonClassInfo*) dev->classes[j])->num_buttons);
		if (dev->classes[j]->type == XIValuatorClass) numAxis++;
	}

        PsychSetStructArrayDoubleElement("totalElements",	deviceIndex, 	(double) numKeys + numAxis, deviceStruct);
        PsychSetStructArrayDoubleElement("features",		deviceIndex, 	(double) dev->num_classes, deviceStruct);
        PsychSetStructArrayDoubleElement("inputs",		deviceIndex, 	(double) numKeys + numAxis, deviceStruct);
        PsychSetStructArrayDoubleElement("outputs",		deviceIndex, 	(double) 0, deviceStruct);
	PsychSetStructArrayDoubleElement("collections",         deviceIndex, 	(double) 0, deviceStruct);
        PsychSetStructArrayDoubleElement("axes",		deviceIndex, 	(double) numAxis, deviceStruct);
        PsychSetStructArrayDoubleElement("buttons",		deviceIndex, 	(double) numKeys, deviceStruct);
        PsychSetStructArrayDoubleElement("hats",		deviceIndex, 	(double) 0, deviceStruct);
        PsychSetStructArrayDoubleElement("sliders",		deviceIndex, 	(double) 0, deviceStruct);
        PsychSetStructArrayDoubleElement("dials",		deviceIndex, 	(double) 0, deviceStruct);
        PsychSetStructArrayDoubleElement("wheels",		deviceIndex, 	(double) 0, deviceStruct);
	
	deviceIndex++;
    }
}

PsychError PsychHIDOSKbCheck(int deviceIndex, double* scanList)
{
	PsychNativeBooleanType* buttonStates;
	unsigned char keys_return[32];
	int keysdown;
	double timestamp;
	int i, j;

	// Map "default" deviceIndex to legacy "Core protocol" method of querying keyboard
	// state. This will give us whatever X has setup as default keyboard:
	if (deviceIndex == INT_MAX) {
		// Request current keyboard state of default keyboard from X-Server:
		XQueryKeymap(dpy, keys_return);
	} else if (deviceIndex < 0 || deviceIndex >= ndevices) {
		PsychErrorExitMsg(PsychError_user, "Invalid keyboard deviceIndex specified. No such device!");
	} else if (info[deviceIndex].use == XIMasterKeyboard) {
		// Master keyboard:

		// Query current client pointer assignment, then switch it to
		// associated master pointer for the master keyboard we want
		// to query. This way, all future queries will query our requested
		// master keyboard:
		j = -1;
		if (!XIGetClientPointer(dpy, None, &j) || (j != info[deviceIndex].attachment)) XISetClientPointer(dpy, None, info[deviceIndex].attachment);

		// Request current keyboard state from X-Server:
		XQueryKeymap(dpy, keys_return);

		// Reset master pointer/keyboard assignment to pre-query state:
		if ((j > 0) && (j != info[deviceIndex].attachment)) XISetClientPointer(dpy, None, j);
	} else {
		// Non-Default deviceIndex: Want to query specific slave keyboard.
		// Validate it maps to a slave keyboard device, as we can't handle
		// master keyboard devices this way and don't want to touch anything
		// but a keyboard'ish device:
		if (info[deviceIndex].use != XISlaveKeyboard) {
			PsychErrorExitMsg(PsychError_user, "Invalid keyboard deviceIndex specified. Not a slave keyboard device!");
		}

		// Open connection to slave keyboard device:
		XDevice* mydev = GetXDevice(deviceIndex);

		// Query its current state:
		XDeviceState* state = XQueryDeviceState(dpy, mydev);
		XInputClass* data = state->data;

		// printf("Dummy = %i , NClasses = %i\n", dummy1, state->num_classes);

		// Find state structure with key status info:
		for (i = 0; i < state->num_classes; i++) {
			// printf("Class %i: Type %i - %i\n", i, (int) data->class, (int) data->length);
			if (data->class == KeyClass) {
				// printf("NumKeys %i\n", ((XKeyState*) data)->num_keys);

				// Copy 32 Byte keystate vector into key_return. Each bit encodes for one key:
				memcpy(&keys_return[0], &(((XKeyState*) data)->keys[0]), sizeof(keys_return));
			}

			data = (XInputClass*) (((void*) data) + ((size_t) data->length));
		}

		XFreeDeviceState(state);
	}

	// Done with query. We have keyboard state in keys_return[] now.

	// Request current time of query:
	PsychGetAdjustedPrecisionTimerSeconds(&timestamp);

	// Reset overall key state to "none pressed":
	keysdown = 0;

	// Any key down?
	for (i = 0; i < 32; i++) keysdown+=(unsigned int) keys_return[i];

	// Copy out overall keystate:
	PsychCopyOutDoubleArg(1, kPsychArgOptional, (keysdown > 0) ? 1 : 0);

	// Copy out timestamp:
	PsychCopyOutDoubleArg(2, kPsychArgOptional, timestamp);

	// Copy keyboard state:
	PsychAllocOutBooleanMatArg(3, kPsychArgOptional, 1, 256, 1, &buttonStates);

	// Map 32 times 8 bitvector to 256 element return vector:
	for(i = 0; i < 32; i++) {
		for(j = 0; j < 8; j++) {
			// This key down?
			buttonStates[i*8 + j] = (PsychNativeBooleanType) (keys_return[i] & (1<<j)) ? 1 : 0;
			// Apply scanList mask, if any provided:
			if (scanList && (scanList[i*8 + j] <= 0)) buttonStates[i*8 + j] = (PsychNativeBooleanType) 0;
		}
	}

	return(PsychError_none);
}

PsychError PsychHIDOSGamePadAxisQuery(int deviceIndex, int axisId, double* min, double* max, double* val, char* axisLabel)
{
	XIDeviceInfo *dev = NULL;
	XIAnyClassInfo *classes;
	int i, j, dummy1, nclasses;

	dev = XIQueryDevice(dpy, info[deviceIndex].deviceid, &dummy1);

	printf("Dummy = %i , NClasses = %i\n", dummy1, dev->num_classes);
	for (i = 0; i < dev->num_classes; i++) {
		printf("Class %i: Type %i\n", i, (int) dev->classes[i]->type);
		if (dev->classes[i]->type == XIKeyClass) printf("NumKeys %i\n", ((XIKeyClassInfo*) dev->classes[i])->num_keycodes);
		if (dev->classes[i]->type == XIButtonClass) printf("NumButtons %i\n", ((XIButtonClassInfo*) dev->classes[i])->num_buttons);
		if (dev->classes[i]->type == XIValuatorClass) {
			printf("Value %f\n", (float) ((XIValuatorClassInfo*) dev->classes[i])->value);
			if (val) *val = (double) ((XIValuatorClassInfo*) dev->classes[i])->value;
		}
	}

	XIFreeDeviceInfo(dev);

	// Open connection to slave keyboard device:
	XDevice* mydev = GetXDevice(deviceIndex);

	// Query its current state:
	XDeviceState* state = XQueryDeviceState(dpy, mydev);

	printf("NClasses = %i\n", state->num_classes);

	// Find state structure with key status info:
	XInputClass* data = state->data;
	for (i = 0; i < state->num_classes; i++) {		 
		printf("Class = %i\n", (int) data->class);
		if (data->class == ValuatorClass) {
			XValuatorState* valuator = (XValuatorState*) data;
			printf("NumAxis %i [Mode=%s]\n", valuator->num_valuators, (valuator->mode == Absolute) ? "Absolute" : "Relative");
			for (j = 0 ; j < valuator->num_valuators; j++) printf("Axis %i: %i\n", j, valuator->valuators[j]);
		}

		data = (XInputClass*) (((void*) data) + ((size_t) data->length));
	}

	XFreeDeviceState(state);

	return(PsychError_none);
}

// This is the event dequeue & process function which updates
// Keyboard queue state. It can be called with 'blockingSinglepass'
// set to TRUE to process exactly one event, if called from the
// background keyboard queue processing thread. Alternatively it
// can be called synchronously from KbQueueCheck with a setting of FALSE
// to iterate over all available events and process them instantaneously:
void KbQueueProcessEvents(psych_bool blockingSinglepass)
{
	double tnow;

	while (1) {
		XGenericEventCookie *cookie = &KbQueue_xevent.xcookie;
		PsychGetAdjustedPrecisionTimerSeconds(&tnow);

		// Single pass or multi-pass?
		if (blockingSinglepass) {
			// Wait until at least one event available and dequeue it:
			XNextEvent(dpy, &KbQueue_xevent);
		} else {
			// Check if event available, dequeue it, if so. Abort
			// processing if no new event available, aka queue empty:
			if (!XCheckTypedEvent(dpy, GenericEvent, &KbQueue_xevent)) break;
		}

		// Is this an event we're interested in?
		if ((cookie->type == GenericEvent) && (cookie->extension == xi_opcode)) {
			// Yes. Process it:
			if (XGetEventData(dpy, cookie)) {
				// Process it:
				printf("Event type %d received\n", cookie->evtype);


				// Release event data:
				XFreeEventData(dpy, cookie);
			}
		}

		// Done if we were only supposed to handle one event, which we did:
		if (blockingSinglepass) break;
	}

	return;
}

// Async processing thread for keyboard events:
void* KbQueueWorkerThreadMain(void* dummy)
{
	while(1) {
		PsychLockMutex(&KbQueueMutex);

		// Check if we should terminate:
		if (KbQueueThreadTerminate) break;

		// Perfrom X-Event processing until no more events are pending:
		KbQueueProcessEvents(TRUE);

		PsychUnlockMutex(&KbQueueMutex);

		// Sleep for 2 msecs before next scan iteration:
		PsychWaitIntervalSeconds(0.002);
	}

	// Done. Unlock the mutex:
	PsychUnlockMutex(&KbQueueMutex);

	// Return and terminate:
	return(NULL);
}

PsychError PsychHIDOSKbQueueCreate(int deviceIndex, int numScankeys, int* scanKeys)
{
	XIDeviceInfo* dev = NULL;
	// int numKeys, j;

	// Valid number of keys?
	if (numScankeys != 256) {
		PsychErrorExitMsg(PsychError_user, "Second argument to KbQueueCreate must be a vector with 256 elements.");
	}

	if (deviceIndex < 0) {
		// Find 
		for(deviceIndex = 0; deviceIndex < ndevices; deviceIndex++) {
			dev = &info[deviceIndex];
			if ((dev->use == XIMasterKeyboard) || (dev->use == XISlaveKeyboard)) break;
		}

		// Nothing found? If so, abort:
		if (deviceIndex >= ndevices) PsychErrorExitMsg(PsychError_user, "Could not find any useable keyboard device!");

		// Ok, deviceIndex now contains our default keyboard to use - The first suitable keyboard.
	} else if (deviceIndex >= ndevices) {
		// Out of range index:
		PsychErrorExitMsg(PsychError_user, "Invalid keyboard 'deviceIndex' specified. No such device!");
	} 

	// Do we finally have a valid keyboard?
	dev = &info[deviceIndex];
	if ((dev->use != XIMasterKeyboard) && (dev->use != XISlaveKeyboard)) {
			PsychErrorExitMsg(PsychError_user, "Invalid keyboard 'deviceIndex' specified. Not a keyboard device!");
	}

	/* This would detect the number of keys on the keyboard:
	numKeys = 0;
	for (j = 0; j < dev->num_classes; j++) {
		if (dev->classes[j]->type == XIKeyClass) numKeys += (int) (((XIKeyClassInfo*) dev->classes[j])->num_keycodes);
	}
	*/

	// Keyboard queue for this deviceIndex already created?
	if (psychHIDKbQueueFirstPress[deviceIndex]) {
		// Yep. Release it, so we can start from scratch:
		PsychHIDOSKbQueueRelease(deviceIndex);
	}

	// Allocate and zero-init memory for tracking key presses and key releases:
	psychHIDKbQueueFirstPress[deviceIndex]   = calloc(256, sizeof(double));
	psychHIDKbQueueFirstRelease[deviceIndex] = calloc(256, sizeof(double));
	psychHIDKbQueueLastPress[deviceIndex]    = calloc(256, sizeof(double));
	psychHIDKbQueueLastRelease[deviceIndex]  = calloc(256, sizeof(double));

	// Ready to use this keybord queue.
	return(PsychError_none);
}

void PsychHIDOSKbQueueRelease(int deviceIndex)
{
	if ((deviceIndex < 0) || (deviceIndex >= ndevices)) {
		// Out of range index:
		PsychErrorExitMsg(PsychError_user, "Invalid keyboard 'deviceIndex' specified. No such device!");
	}

	// Keyboard queue for this deviceIndex already exists?
	if (NULL == psychHIDKbQueueFirstPress[deviceIndex]) {
		// No. Nothing to do then.
		return;
	}

	// Ok, we have a keyboard queue. Stop any operation on it first:
	PsychHIDOSKbQueueStop(deviceIndex);

	// Release its data structures:
	free(psychHIDKbQueueFirstPress[deviceIndex]); psychHIDKbQueueFirstPress[deviceIndex] = NULL;
	free(psychHIDKbQueueFirstRelease[deviceIndex]); psychHIDKbQueueFirstRelease[deviceIndex] = NULL;
	free(psychHIDKbQueueLastPress[deviceIndex]); psychHIDKbQueueLastPress[deviceIndex] = NULL;
	free(psychHIDKbQueueLastRelease[deviceIndex]); psychHIDKbQueueLastRelease[deviceIndex] = NULL;

	// Done.
	return;
}

void PsychHIDOSKbQueueStop(int deviceIndex)
{
	psych_bool queueActive;
	int i;

	XIDeviceInfo* dev = NULL;

	if ((deviceIndex < 0) || (deviceIndex >= ndevices)) {
		// Out of range index:
		PsychErrorExitMsg(PsychError_user, "Invalid keyboard 'deviceIndex' specified. No such device!");
	}

	// Keyboard queue for this deviceIndex already exists?
	if (NULL == psychHIDKbQueueFirstPress[deviceIndex]) {
		// No. Nothing to do then.
		return;
	}

	// Keyboard queue already stopped?
	if (!psychHIDKbQueueActive[deviceIndex]) return;

	// Queue is active. Stop it:
	PsychLockMutex(&KbQueueMutex);

	// Setup event mask, so events from our associated xinput device
	// do not reach our event queue:
	XIEventMask emask;
	unsigned char mask[(XI_LASTEVENT + 7)/8];

	// Clear the event mask. That should to the trick:
	memset(mask, 0, sizeof(mask));
	// XISetMask(mask, XI_KeyPress);
	// XISetMask(mask, XI_KeyRelease);

	emask.deviceid = info[deviceIndex].deviceid;
	emask.mask_len = sizeof(mask);
	emask.mask = mask;
	XISelectEvents(dpy, DefaultRootWindow(dpy), &emask, 1);

	// Mark queue logically stopped:
	psychHIDKbQueueActive[deviceIndex] = FALSE;

	PsychUnlockMutex(&KbQueueMutex);

	// Was this the last active queue?
	queueActive = FALSE;
	for (i = 0; i < PSYCH_HID_MAX_DEVICES; i++) {
		queueActive |= psychHIDKbQueueActive[i];
	}

	// If more queues are active then we're done:
	if (queueActive) return;

	// No more active queues. Shutdown the common processing thread:
	PsychLockMutex(&KbQueueMutex);
	KbQueueThreadTerminate = TRUE;

	// Send some event to unblock the thread:
	// FIXME TODO

	// Shutdown the thread, wait for its termination:
	PsychDeleteThread(&KbQueueThread);
	KbQueueThreadTerminate = FALSE;

	// Done.
	PsychUnlockMutex(&KbQueueMutex);

	return;
}

void PsychHIDOSKbQueueStart(int deviceIndex)
{
	psych_bool queueActive;
	int i;

	if ((deviceIndex < 0) || (deviceIndex >= ndevices)) {
		// Out of range index:
		PsychErrorExitMsg(PsychError_user, "Invalid keyboard 'deviceIndex' specified. No such device!");
	}

	// Does Keyboard queue for this deviceIndex already exist?
	if (NULL == psychHIDKbQueueFirstPress[deviceIndex]) {
		// No. Bad bad...
		printf("PsychHID-ERROR: Tried to start processing on non-existent keyboard queue for deviceIndex %i! Call KbQueueCreate first!\n", deviceIndex);
		PsychErrorExitMsg(PsychError_user, "Invalid keyboard 'deviceIndex' specified. No queue for that device yet!");
	}

	// Keyboard queue already stopped? Then we ain't nothing to do:
	if (psychHIDKbQueueActive[deviceIndex]) return;

	// Queue is inactive. Start it:

	// Will this be the first active queue, ie., aren't there any queues running so far?
	queueActive = FALSE;
	for (i = 0; i < PSYCH_HID_MAX_DEVICES; i++) {
		queueActive |= psychHIDKbQueueActive[i];
	}

	PsychLockMutex(&KbQueueMutex);

	// Setup event mask, so events from our associated xinput device
	// get enqueued in our event queue:
	XIEventMask emask;
	unsigned char mask[(XI_LASTEVENT + 7)/8];

	memset(mask, 0, sizeof(mask));
	XISetMask(mask, XI_KeyPress);
	XISetMask(mask, XI_KeyRelease);

	// XISetMask(mask, XI_ButtonPress);
	// XISetMask(mask, XI_ButtonRelease);
	// XISetMask(mask, XI_Motion);

	emask.deviceid = info[deviceIndex].deviceid;
	emask.mask_len = sizeof(mask);
	emask.mask = mask;
	XISelectEvents(dpy, DefaultRootWindow(dpy), &emask, 1);

	// Mark this queue as logically started:
	psychHIDKbQueueActive[deviceIndex] = TRUE;

	// Queue started.
	PsychUnlockMutex(&KbQueueMutex);
	

	// If other queues are already active then we're done:
	if (queueActive) return;

	// No other active queues. We are the first one.

	// Start the common processing thread for all queues:
	PsychLockMutex(&KbQueueMutex);
	KbQueueThreadTerminate = FALSE;

	if (PsychCreateThread(&KbQueueThread, NULL, KbQueueWorkerThreadMain, NULL)) {
		// We are soo screwed:

		// Cleanup the mess:
		psychHIDKbQueueActive[deviceIndex] = FALSE;
		PsychUnlockMutex(&KbQueueMutex);

		// Whine a little bit:
		printf("PsychHID-ERROR: Start of keyboard queue processing failed!\n");
		PsychErrorExitMsg(PsychError_system, "Creation of keyboard queue background processing thread failed!");
	}

	// Up and running, we're done!
	PsychUnlockMutex(&KbQueueMutex);

	return;
}

