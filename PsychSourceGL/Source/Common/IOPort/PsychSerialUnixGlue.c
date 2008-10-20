/*
	Psychtoolbox3/PsychSourceGL/Source/Common/IOPort/PsychSerialUnixGlue.c		
    
	PROJECTS: 
	
		IOPort for now.
  
	AUTHORS:
	
		mario.kleiner at tuebingen.mpg.de	mk
  
	PLATFORMS:	
	
		All Unix (aka Posix) systems: OS/X and Linux.
    
	HISTORY:

		04/10/2008	mk		Initial implementation.
 
	DESCRIPTION:
	
		This is the operating system dependent "glue code" layer for access to serial ports for the
		Unices, ie., for GNU/Linux and Apple MacOS/X. It is used by the higher-level serial port
		routines to abstract out operating system dependencies.

		The code is shared and #ifdef'ed if needed, because most of it is identical for Linux and OS/X.
*/

#include "Psych.h" 

// Need to exclude this files source-code from compile on Win32:
#if PSYCH_SYSTEM != PSYCH_WINDOWS

#include "IOPort.h"

// Externally defined level of verbosity:
extern int verbosity;

// Map numeric baud rate to Posix constant:
static int BaudToConstant(int inint)
{
	// Need to map from numeric values to predefined constants. On OS/X the
	// constants are identical to the input values, but on Linux they aren't, so better safe
	// than sorry, although it looks as if Linux accepts the "raw" numbers properly as well...
	switch(inint) {
		case 50: inint = B50; break;
		case 75: inint = B75; break;
		case 110: inint = B110; break;
		case 134: inint = B134; break;
		case 150: inint = B150; break;
		case 200: inint = B200; break;
		case 300: inint = B300; break;
		case 600: inint = B600; break;
		case 1200: inint = B1200; break;
		case 1800: inint = B1800; break;
		case 2400: inint = B2400; break;
		case 4800: inint = B4800; break;
		case 9600: inint = B9600; break;
		case 19200: inint = B19200; break;
		case 38400: inint = B38400; break;
		case 57600: inint = B57600; break;
		case 115200: inint = B115200; break;
		case 230400: inint = B230400; break;
		#if PSYCH_SYSTEM == PSYCH_OSX
		// Only defined on OS/X:
		case 7200: inint = B7200; break;
		case 14400: inint = B14400; break;
		case 28800: inint = B28800; break;
		case 76800: inint = B76800; break;
		#endif

		default:
			// Pass inint unchanged in the hope that the OS will accept it:
			if (verbosity > 1) printf("IOPort: Non-standard BaudRate %i specified. We'll see if the OS likes this. Double-check the settings!\n", inint);
	}
	
	return(inint);
}

// Map Posix constant to numeric baud rate:
static int ConstantToBaud(int inint)
{
	// Need to map predefined constants to numeric values On OS/X the
	// constants are identical to the input values, but on Linux they aren't, so better safe
	// than sorry, although it looks as if Linux accepts the "raw" numbers properly as well...
	switch(inint) {
		case B50: inint = 50; break;
		case B75: inint = 75; break;
		case B110: inint = 110; break;
		case B134: inint = 134; break;
		case B150: inint = 150; break;
		case B200: inint = 200; break;
		case B300: inint = 300; break;
		case B600: inint = 600; break;
		case B1200: inint = 1200; break;
		case B1800: inint = 1800; break;
		case B2400: inint = 2400; break;
		case B4800: inint = 4800; break;
		case B9600: inint = 9600; break;
		case B19200: inint = 19200; break;
		case B38400: inint = 38400; break;
		case B57600: inint = 57600; break;
		case B115200: inint = 115200; break;
		case B230400: inint = 230400; break;
		#if PSYCH_SYSTEM == PSYCH_OSX
		// Only defined on OS/X:
		case B7200: inint = 7200; break;
		case B14400: inint = 14400; break;
		case B28800: inint = 28800; break;
		case B76800: inint = 76800; break;
		#endif
		default:
			// Pass inint unchanged in the hope that the OS will accept it:
			if (verbosity > 1) printf("IOPort: Non-standard BaudRate %i detected. Let's see if this makes sense...\n", inint);
	}
	
	return(inint);
}

int PsychSerialUnixGlueAsyncReadbufferBytesAvailable(PsychSerialDeviceRecord* device)
{	
	int navail = 0;
	
	// Lock data structure:
	pthread_mutex_lock(&(device->readerLock));
	
	// Compute amount of pending data for readout:
	navail = (device->readerThreadWritePos - device->clientThreadReadPos);

	// Unlock data structure:
	pthread_mutex_unlock(&(device->readerLock));
		
	// Return it.
	return(navail);
}

void* PsychSerialUnixGlueReaderThreadMain(void* deviceToCast)
{
	int rc, nread, oldstate;
	int navail;
	

	// Get a handle to our device struct: These pointers must not be NULL!!!
	PsychSerialDeviceRecord* device = (PsychSerialDeviceRecord*) deviceToCast;
	
	// Main loop: Runs until external thread cancellation:
	while (1) {
		// Enough data available for read of requested granularity?
		// If not, we sleep for some time, then retry:
		ioctl(device->fileDescriptor, FIONREAD, &navail);

		while(navail < device->readGranularity) {
			pthread_testcancel();
			PsychWaitIntervalSeconds(device->pollLatency);
			ioctl(device->fileDescriptor, FIONREAD, &navail);
		}
		
		// Enough data available. Read it!
		if ((nread = read(device->fileDescriptor, &(device->readBuffer[(device->readerThreadWritePos) % (device->readBufferSize)]), device->readGranularity)) != device->readGranularity) {
			// Should not happen...
			fprintf(stderr, "PTB-ERROR: In IOPort:PsychSerialUnixGlueReaderThreadMain(): Failed to read %i bytes of data for unknown reason (Got only %i bytes)! Padding...\n", device->readGranularity, nread);
		}

		// Prevent our cancellation:
		pthread_setcancelstate(PTHREAD_CANCEL_DISABLE, &oldstate);
		
		// Try to lock, block until available if not available:
		if ((rc=pthread_mutex_lock(&(device->readerLock)))) {
			// This could potentially kill Matlab, as we're printing from outside the main interpreter thread.
			// Use fprintf() instead of the overloaded printf() (aka mexPrintf()) in the hope that we don't
			// wreak havoc -- maybe it goes to the system log, which should be safer...
			fprintf(stderr, "PTB-ERROR: In IOPort:PsychSerialUnixGlueReaderThreadMain(): mutex_lock failed  [%s].\n", strerror(rc));
			
			// Commit suicide:
			return;
		}
		
		// Update linear write pointer:
		device->readerThreadWritePos += device->readGranularity;
		
		// Need to unlock the mutex:
		if ((rc=pthread_mutex_unlock(&(device->readerLock)))) {
			// This could potentially kill Matlab, as we're printing from outside the main interpreter thread.
			// Use fprintf() instead of the overloaded printf() (aka mexPrintf()) in the hope that we don't
			// wreak havoc -- maybe it goes to the system log, which should be safer...
			fprintf(stderr, "PTB-ERROR: In IOPort:PsychSerialUnixGlueReaderThreadMain(): Last mutex_unlock in termination failed  [%s].\n", strerror(rc));
			
			// Commit suicide:
			return;
		}
		
		// Reenable cancellation:
		pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, &oldstate);
		
		// Test for explicit cancellation by mother-thread:
		pthread_testcancel();

		// Next iteration...
	}
	
	// Go and die peacefully...
	return;
}

void PsychIOOSShutdownSerialReaderThread(PsychSerialDeviceRecord* device)
{
	if (device->readerThread) {
		// Make sure we don't hold the mutex:
		pthread_mutex_unlock(&(device->readerLock));

		// Cancel the thread:
		pthread_cancel(device->readerThread);
		
		// Wait for it to die:
		pthread_join(device->readerThread, NULL);
		
		// Mark it as dead:
		device->readerThread = NULL;
		
		// Release the mutex:
		pthread_mutex_destroy(&(device->readerLock));
	}
	
	return;
}

/* PsychIOOSOpenSerialPort()
 *
 * Open a serial port device and configure it.
 *
 * portSpec - String with the port name / device name of the serial port device.
 * configString - String with port configuration parameters.
 * errmsg - Pointer to char[] buffer in which error messages should be returned, if any.
 * On success, allocate a PsychSerialDeviceRecord with all relevant settings,
 * return a pointer to it.
 *
 * Otherwise abort with error message.
 */
PsychSerialDeviceRecord* PsychIOOSOpenSerialPort(const char* portSpec, const char* configString, char* errmsg)
{
    int				fileDescriptor = -1;
    struct termios	options;
	PsychSerialDeviceRecord* device = NULL;
	bool			usererr = FALSE;
	
	// Init errmsg error message to empty == no error:
	errmsg[0] = 0;
	
    // Open the serial port read/write, with no controlling terminal, and don't wait for a connection.
    // The O_NONBLOCK flag also causes subsequent I/O on the device to be non-blocking.
    // See open(2) ("man 2 open") for details.
    fileDescriptor = open(portSpec, O_RDWR | O_NOCTTY | O_NONBLOCK);
    if (fileDescriptor == -1)
    {
		if (errno == ENOENT) {
			sprintf(errmsg, "Error opening serial port device %s - No such serial port device exists! (%d) [ENOENT].\n", portSpec, errno);
			usererr = TRUE;
		}		
		else if (errno == EBUSY || errno == EPERM) {
			sprintf(errmsg, "Error opening serial port device %s - The serial port is already open, close it first! %s(%d) [EBUSY EPERM]. Could be a permission problem as well.\n", portSpec, strerror(errno), errno);
			usererr = TRUE;
		}
		else {
			sprintf(errmsg, "Error opening serial port device %s - %s(%d).\n", portSpec, strerror(errno), errno);
		}
        goto error;
    }

    // Note that open() follows POSIX semantics: multiple open() calls to the same file will succeed
    // unless the TIOCEXCL ioctl is issued. This will prevent additional opens except by root-owned
    // processes.
    // See tty(4) ("man 4 tty") and ioctl(2) ("man 2 ioctl") for details.
    if (ioctl(fileDescriptor, TIOCEXCL) == -1)
    {
        if (verbosity > 1) printf("IOPort-Warning: Error setting exclusive access (TIOCEXCL) on device %s - %s(%d).\n", portSpec, strerror(errno), errno);
        // goto error;
    }
    
    // Create the device struct and init it:
	device = calloc(1, sizeof(PsychSerialDeviceRecord));
	device->fileDescriptor = -1;
	device->readBuffer = NULL;
	device->readBufferSize = 0;
	device->readerThread = NULL;
	
    // Get the current options and save them so we can restore the default settings later.
    if (tcgetattr(fileDescriptor, &(device->OriginalTTYAttrs)) == -1) {
        sprintf(errmsg, "Error getting original device settings for device %s - %s(%d).\n", portSpec, strerror(errno), errno);
        goto error;
    }

	strncpy(device->portSpec, portSpec, sizeof(device->portSpec));
	device->fileDescriptor = fileDescriptor;
    options = device->OriginalTTYAttrs;

	// Ok, the device is basically open. Call the reconfiguration routine with the setting
	// string. It will do all further setup work:
	if (PsychError_none != PsychIOOSConfigureSerialPort(device, configString)) {
		// Something failed. Error handling...
        sprintf(errmsg, "Error changing device settings for device %s - %s(%d).\n", portSpec, strerror(errno), errno);
		usererr = TRUE;
        goto error;
	}

	if (device->readBuffer == NULL) {
        sprintf(errmsg, "Error for device %s - No InputBuffer allocated! You must specify the 'InputBuffer' size in the configuration.\n", portSpec);
		usererr = TRUE;
        goto error;
	}

	// printf("MAX_INPUT = %i / %i \n", (int) fpathconf(device->fileDescriptor, _PC_MAX_INPUT), (int) fpathconf(device->fileDescriptor, _PC_MAX_CANON));

    // Success! Return Pointer to new device structure:
    return(device);
    
    // Failure path: Called on error with error message in errmsg:
error:

    if (fileDescriptor != -1)
    {
		// File open.
		if (device) {
			// Settings retrieved or made?
			if (device->fileDescriptor != -1) {
				// Have original settings available: Try to restore them:
				if (tcsetattr(fileDescriptor, TCSANOW, &options) == -1) {
					if (verbosity > 1) printf("WARNING: In error handling: Error restoring tty attributes %s - %s(%d).\n", portSpec, strerror(errno), errno);
				}
			}

			// Release read buffer, if any:
			if (device->readBuffer) free(device->readBuffer);

			// Release device struct:
			free(device);
		}

		// Close file:
        close(fileDescriptor);
    }
    
	// Return with error message:
	if (verbosity > 0) {
		PsychErrorExitMsg(((usererr) ? PsychError_user : PsychError_system), errmsg);
	}
		
	return(NULL);
}

/* PsychIOOSCloseSerialPort()
 *
 * Close serial port connection/device referenced by given 'device' record.
 * Free the device record afterwards.
 */
void PsychIOOSCloseSerialPort(PsychSerialDeviceRecord* device)
{
	if (device == NULL) PsychErrorExitMsg(PsychError_internal, "NULL-Ptr instead of valid device pointer!");
	
	PsychIOOSShutdownSerialReaderThread(device);
	
	// Drain all send-buffers:
    // Block until all written output has been sent from the device.
    // Note that this call is simply passed on to the serial device driver. 
	// See tcsendbreak(3) ("man 3 tcsendbreak") for details.
    if (tcdrain(device->fileDescriptor) == -1)
    {
        if (verbosity > 1) printf("IOPort: WARNING: While trying to close serial port: Error waiting for drain - %s(%d).\n", strerror(errno), errno);
    }
    
    // Traditionally it is good practice to reset a serial port back to
    // the state in which you found it. This is why the original termios struct
    // was saved.
    if (tcsetattr(device->fileDescriptor, TCSANOW, &(device->OriginalTTYAttrs)) == -1)
    {
        if (verbosity > 1) printf("IOPort: WARNING: While trying to close serial port: Could not restore original port settings - %s(%d).\n", strerror(errno), errno);
    }

	// Close device:
    close(device->fileDescriptor);

	// Release read buffer, if any:
	if (device->readBuffer) free(device->readBuffer);

	// Release memory for device struct:
	free(device);
	
	// Done.
	return;
}

/* PsychIOOSConfigureSerialPort()
 *
 * (Re-)configure serial port connection/device referenced by given 'device' record.
 * The relevant parameters are stored in 'configString'.
 *
 * Returns success- or failure status.
 */
PsychError PsychIOOSConfigureSerialPort(PsychSerialDeviceRecord* device, const char* configString)
{
	int				rc;
    struct termios	options;
    int				handshake;
	char*			p;
	float			infloat;
	int				inint;
	unsigned long	mics = 0UL;
	bool			updatetermios = FALSE;

    // The serial port attributes such as timeouts and baud rate are set by modifying the termios
    // structure and then calling tcsetattr() to cause the changes to take effect. Note that the
    // changes will not become effective without the tcsetattr() call.
    // See tcsetattr(4) ("man 4 tcsetattr") for details.

	// Retrieve current settings:
    if (tcgetattr(device->fileDescriptor, &options) == -1)
    {
        if (verbosity > 0) printf("Error getting current serial port device settings for device %s - %s(%d).\n", device->portSpec, strerror(errno), errno);
		return(PsychError_system);
    }
	
    // Print the current input and output baud rates.
    // See tcsetattr(4) ("man 4 tcsetattr") for details.
    if (verbosity > 3) {
		printf("IOPort-Info: Configuration for device %s:\n", device->portSpec);
		printf("IOPort-Info: Current input baud rate is %d\n", (int) cfgetispeed(&options));
		printf("IOPort-Info: Current output baud rate is %d\n", (int) cfgetospeed(&options));
    }
	
    // Set raw input (non-canonical) mode: No line-based processing or editing of
	// received data via special control characters, unless a processing mode of
	// cooked was explicitely requested:
    if ((strstr(configString, "ProcessingMode=")!=NULL) && (strstr(configString, "ProcessingMode=Cooked") == NULL)) { cfmakeraw(&options); updatetermios = TRUE; }
	
	// No minimum number of bytes to receive:
	options.c_cc[VMIN] = 0;

	if ((p = strstr(configString, "ReceiveTimeout="))) {
		// Set timeouts for read operations:
		// See tcsetattr(4) ("man 4 tcsetattr") and termios(4) ("man 4 termios") for details.		
		if ((1!=sscanf(p, "ReceiveTimeout=%f", &infloat)) || (infloat < 0)) {
			if (verbosity > 0) printf("Invalid parameter for ReceiveTimeout set! Typo, or negative value provided.\n");
			return(PsychError_user);
		}
		else {
			// Set timeout: It is in granularity of 1/10th seconds, so we need to quantize to 10th of seconds:
			options.c_cc[VTIME] = (int) (infloat * 10 + 0.5);
			// Clamp to a minimum of 0.1 secs if a non-zero timeout is requested:
			if ((options.c_cc[VTIME] == 0) && (infloat > 0)) {
				if (verbosity > 1) printf("IOPort: Warning: Requested per-byte 'ReceiveTimeout' value %f secs is positive but smaller than supported minimum of 0.1 secs on Unix. Changed value to 0.1 secs.\n", infloat);
				infloat = 0.1;
				options.c_cc[VTIME] = 1;
			}
			device->readTimeout = infloat;
			updatetermios = TRUE;
		}
	}
	
	// Set common baud rate for send and receive:
	if ((p = strstr(configString, "BaudRate="))) {
		if (1!=sscanf(p, "BaudRate=%i", &inint)) {
			if (verbosity > 0) printf("Invalid parameter for BaudRate set!\n");
			return(PsychError_invalidIntegerArg);
		}
		else {
			// Set common speed for input- and output queues:
			inint = BaudToConstant(inint);
			if (-1==cfsetspeed(&options, inint)) {
				if (verbosity > 0) printf("Invalid BaudRate %i not accepted! (%s)", inint, strerror(errno));
				return(PsychError_invalidIntegerArg);
			}
			updatetermios = TRUE;
		}
	}

	// Generation and handling of parity bits:
	if ((p = strstr(configString, "Parity="))) {
		// Clear all parity settings:
		options.c_cflag &= ~(PARENB | PARODD);
		options.c_iflag &= ~INPCK;
		
		if (strstr(configString, "Parity=Even")) {
			options.c_cflag |= PARENB;
			options.c_iflag |= INPCK;
		}
		else 
		if (strstr(configString, "Parity=Odd")) {
			options.c_cflag |= (PARENB | PARODD);
			options.c_iflag |= INPCK;
		}
		else
		if (strstr(configString, "Parity=None")) {
			// Nothing to do.
		}
		else {
			// Invalid parity spec:
			if (verbosity > 0) printf("Invalid parity setting %s not accepted! (Valid are None, Even and Odd", p);
			return(PsychError_user);
		}
		updatetermios = TRUE;		
	}
	
	// Handling of Break conditions:
	if ((p = strstr(configString, "BreakBehaviour="))) {
		// Clear all break settings:
		options.c_iflag &= ~(IGNBRK | BRKINT);
		
		if (strstr(configString, "BreakBehaviour=Ignore")) {
			// Break signals will be ignored:
			options.c_iflag |= IGNBRK;
		}
		else 
		if (strstr(configString, "BreakBehaviour=Flush")) {
			// A break signal will flush the input- and output-queues:
			options.c_iflag |= BRKINT;
		}
		else
		if (strstr(configString, "BreakBehaviour=Zero")) {
			// Don't change anything, ie. IGNBRK and BRKINT not set:
			// A break signal will be output as single zero byte.
		}
		else {
			// Invalid spec:
			if (verbosity > 0) printf("Invalid break behaviour %s not accepted (Valid: Ignore, Flush, or Zero)!", p);
			return(PsychError_user);
		}
		updatetermios = TRUE;
	}

	// Handling of data bits:
	if ((p = strstr(configString, "DataBits="))) {
		// Clear all databit settings:
		options.c_cflag &= ~(CS5 | CS6 | CS7 | CS8);
		
		if (strstr(configString, "DataBits=5")) {
			options.c_cflag |= CS5;
		}
		else 
		if (strstr(configString, "DataBits=6")) {
			options.c_cflag |= CS6;
		}
		else
		if (strstr(configString, "DataBits=7")) {
			options.c_cflag |= CS7;
		}
		else
		if (strstr(configString, "DataBits=8")) {
			options.c_cflag |= CS8;
		}
		else {
			// Invalid spec:
			if (verbosity > 0) printf("Invalid setting for data bits %s not accepted! (Valid: 5, 6, 7 or 8 databits)", p);
			return(PsychError_user);
		}
		updatetermios = TRUE;	
	}
	
	// Handling of stop bits:
	if ((p = strstr(configString, "StopBits="))) {
		// Clear all stopbit settings:
		options.c_cflag &= ~CSTOPB;
		
		if (strstr(configString, "StopBits=1")) {
			// Nothing to do, this is already set above.
		}
		else 
		if (strstr(configString, "StopBits=2")) {
			options.c_cflag |= CSTOPB;
		}
		else {
			// Invalid spec:
			if (verbosity > 0) printf("Invalid setting for stop bits %s not accepted! (Valid: 1 or 2 stopbits)", p);
			return(PsychError_user);
		}
		updatetermios = TRUE;	
	}
	
	// Handling of flow control:
	if ((p = strstr(configString, "FlowControl="))) {
		// Clear all flow control settings:
		options.c_cflag &= ~CRTSCTS;
		options.c_iflag &= ~(IXON | IXOFF);
		
		if (strstr(configString, "FlowControl=None")) {
			// Set above already...
		}
		else 
		if (strstr(configString, "FlowControl=Software")) {
			options.c_iflag |= (IXON | IXOFF);
		}
		else
		if (strstr(configString, "FlowControl=Hardware")) {
			options.c_cflag |= CRTSCTS;
		}
		else {
			// Invalid spec:
			if (verbosity > 0) printf("Invalid setting for flow control %s not accepted! (Valid: None, Software, Hardware)", p);
			return(PsychError_user);
		}
		updatetermios = TRUE;		
	}
			
	//#if defined(MAC_OS_X_VERSION_10_4) && (MAC_OS_X_VERSION_MIN_REQUIRED >= MAC_OS_X_VERSION_10_4)
	//	// Starting with Tiger, the IOSSIOSPEED ioctl can be used to set arbitrary baud rates
	//	// other than those specified by POSIX. The driver for the underlying serial hardware
	//	// ultimately determines which baud rates can be used. This ioctl sets both the input
	//	// and output speed. 
	//	
	//	speed_t speed = 14400; // Set 14400 baud
	//    if (ioctl(fileDescriptor, IOSSIOSPEED, &speed) == -1)
	//    {
	//        printf("Error calling ioctl(..., IOSSIOSPEED, ...) %s - %s(%d).\n",
	//            bsdPath, strerror(errno), errno);
	//    }
	//#endif
    
    // Print the new input and output baud rates. Note that the IOSSIOSPEED ioctl interacts with the serial driver 
	// directly bypassing the termios struct. This means that the following two calls will not be able to read
	// the current baud rate if the IOSSIOSPEED ioctl was used but will instead return the speed set by the last call
	// to cfsetspeed.
    if (verbosity > 3) {
			// Retrieve baudrate and check for equal rate on input- and output queue:
			inint = cfgetispeed(&options);
			if ((inint != cfgetospeed(&options)) && (verbosity > 1)) printf("IOPort: Warning: Hmm, new input- and output baudrates %i vs. %i don't match!? May or may not be a problem...\n", inint, (int) cfgetospeed(&options)); 

			// Output new baud rate:
			printf("IOPort-Info: Baud rate changed to %d\n", (int) ConstantToBaud(inint));
    }
    
    // Cause the new options to take effect immediately.
    if (updatetermios && (tcsetattr(device->fileDescriptor, TCSANOW, &options) == -1))
    {
        if (verbosity > 0) printf("Error setting new serial port configuration attributes for device %s - %s(%d).\n", device->portSpec, strerror(errno), errno);
        return(PsychError_system);
    }

    // To set the modem handshake lines, use the following ioctls.
    // See tty(4) ("man 4 tty") and ioctl(2) ("man 2 ioctl") for details.

	// Retrieve current handshake state aka DTR, RTS, CTS, DSR et al.:
    if (ioctl(device->fileDescriptor, TIOCMGET, &handshake) == -1) {
		if (verbosity > 0) printf("Error getting lines status for device %s - %s(%d).\n", device->portSpec, strerror(errno), errno);
		return(PsychError_system);
    }
    
	if (verbosity > 3) {
		printf("IOPort-Info: Handshake lines currently set to %d : ", handshake);
		printf("DTR=%i : DSR=%i : RTS=%i : CTS=%i\n", (handshake & TIOCM_DTR) ? 1:0, (handshake & TIOCM_DSR) ? 1:0, (handshake & TIOCM_RTS) ? 1:0, (handshake & TIOCM_CTS) ? 1:0);
	}

	// Handling of DTR :
	if ((p = strstr(configString, "DTR="))) {
		if (strstr(configString, "DTR=0")) {
			handshake&= ~TIOCM_DTR;
		}
		else
		if (strstr(configString, "DTR=1")) {
			handshake|= TIOCM_DTR;
		}
		else {
			// Invalid spec:
			if (verbosity > 0) printf("Invalid setting for DTR %s not accepted! (Valid: 1 or 0)", p);
			return(PsychError_user);
		}		
	}

	// Handling of DSR :
	if ((p = strstr(configString, "DSR="))) {
		if (strstr(configString, "DSR=0")) {
			handshake&= ~TIOCM_DSR;
		}
		else
		if (strstr(configString, "DSR=1")) {
			handshake|= TIOCM_DSR;
		}
		else {
			// Invalid spec:
			if (verbosity > 0) printf("Invalid setting for DSR %s not accepted! (Valid: 1 or 0)", p);
			return(PsychError_user);
		}		
	}
	
	// Handling of RTS :
	if ((p = strstr(configString, "RTS="))) {
		if (strstr(configString, "RTS=0")) {
			handshake&= ~TIOCM_RTS;
		}
		else
		if (strstr(configString, "RTS=1")) {
			handshake|= TIOCM_RTS;
		}
		else {
			// Invalid spec:
			if (verbosity > 0) printf("Invalid setting for RTS %s not accepted! (Valid: 1 or 0)", p);
			return(PsychError_user);
		}		
	}

	// Handling of CTS :
	if ((p = strstr(configString, "CTS="))) {
		if (strstr(configString, "CTS=0")) {
			handshake&= ~TIOCM_CTS;
		}
		else
		if (strstr(configString, "CTS=1")) {
			handshake|= TIOCM_CTS;
		}
		else {
			// Invalid spec:
			if (verbosity > 0) printf("Invalid setting for CTS %s not accepted! (Valid: 1 or 0)", p);
			return(PsychError_user);
		}		
	}

    if (ioctl(device->fileDescriptor, TIOCMSET, &handshake) == -1) {
		if (verbosity > 0) printf("Error setting handshake lines status for device %s - %s(%d).\n", device->portSpec, strerror(errno), errno);
		return(PsychError_system);
    }
	
	// This function is OS/X specific, silently ignored on Linux...
#if PSYCH_SYSTEM == PSYCH_OSX	
	if ((p = strstr(configString, "ReceiveLatency="))) {
		// Set receive latency for low-level driver: Minimum latency between dequeue and notify
		// operation on reception of new input data. The granularity is microseconds...
		if ((1!=sscanf(p, "ReceiveLatency=%f", &infloat)) || (infloat < 0)) {
			if (verbosity > 0) printf("Invalid parameter for ReceiveLatency set! Typo, or negative value provided.\n");
			return(PsychError_user);
		}
		else {
			// Set timeout: It is in granularity microseconds, so we need to quantize to microseconds:
			mics = (unsigned long) (infloat * 1000000 + 0.5);
			
			// A value of zero would revert to operating system default, so we need to make sure
			// the mics value is at least 1 microsecond:
			if (mics == 0) mics = 1;
			
			// Set the receive latency in microseconds. Serial drivers use this value to determine how often to
			// dequeue characters received by the hardware. Most applications don't need to set this value: if an
			// app reads lines of characters, the app can't do anything until the line termination character has been
			// received anyway. The most common applications which are sensitive to read latency are MIDI and IrDA
			// applications.			
			if (ioctl(device->fileDescriptor, IOSSDATALAT, &mics) == -1) {
				if (verbosity > 0) printf("Error setting receive latency for device %s to %d microseconds - %s(%d).\n", device->portSpec, mics, strerror(errno), errno);
				return(PsychError_system);
			}
		}
	}
#endif

	// TODO: Try if TIOCTIMESTAMP is useful to get extra accurate receive timestamps on OS/X...

	// Set input buffer size for receive ops:
	if ((p = strstr(configString, "InputBufferSize="))) {
		if ((1!=sscanf(p, "InputBufferSize=%i", &inint)) || (inint < 1)) {
			if (verbosity > 0) printf("Invalid parameter for InputBufferSize set! Typo or requested buffer size smaller than 1 byte.\n");
			return(PsychError_invalidIntegerArg);
		}
		else {
			// Set InputBufferSize:
			if (device->readBuffer) {
				// Buffer already exists. Try to realloc:
				p = (char*) realloc(device->readBuffer, inint);
				if (p == NULL) {
					// Realloc failed:
					if (verbosity > 0) printf("Reallocation of Inputbuffer of device %s for new size %i failed! (%s)", device->portSpec, inint, strerror(errno));
					return(PsychError_outofMemory);
				}
				// Worked. Assign new values:
				device->readBuffer = (unsigned char*) p;
				device->readBufferSize = (unsigned int) inint;
			}
			else {
				// No Buffer yet. Allocate
				p = malloc(inint);
				if (p == NULL) {
					// Realloc failed:
					if (verbosity > 0) printf("Allocation of Inputbuffer of device %s of size %i failed! (%s)", device->portSpec, inint, strerror(errno));
					return(PsychError_outofMemory);
				}
				
				// Worked. Assign new values:
				device->readBuffer = (unsigned char*) p;
				device->readBufferSize = (unsigned int) inint;
			}
		}
	}

	if ((p = strstr(configString, "PollLatency="))) {
		// Set polling latency for busy-waiting read operations:
		if ((1!=sscanf(p, "PollLatency=%f", &infloat)) || (infloat < 0)) {
			if (verbosity > 0) printf("Invalid parameter for PollLatency set! Typo, or negative value provided.\n");
			return(PsychError_user);
		}
		else {
			device->pollLatency = infloat;
		}
	}
	
	// Async background read via parallel thread requested?
	if ((p = strstr(configString, "StartBackgroundRead="))) {
		if (1!=sscanf(p, "StartBackgroundRead=%i", &inint)) {
			if (verbosity > 0) printf("Invalid parameter for StartBackgroundRead set!\n");
			return(PsychError_invalidIntegerArg);
		}
		else {
			// Set data-fetch granularity for single background read requests:
			if (inint < 1) {
				if (verbosity > 0) printf("Invalid StartBackgroundRead fetch granularity of %i bytes provided. Must be at least 1 byte!\n", inint);
				return(PsychError_invalidIntegerArg);
			}
			
			if (device->readBufferSize < inint) {
				if (verbosity > 0) printf("Invalid StartBackgroundRead fetch granularity of %i bytes provided. Bigger than current Inputbuffer size of %i bytes!\n", inint, device->readBufferSize);
				return(PsychError_invalidIntegerArg);
			}

			if ((device->readBufferSize % inint) != 0) {
				if (verbosity > 0) printf("Invalid StartBackgroundRead fetch granularity of %i bytes provided. Inputbuffer size of %i bytes is not an integral multiple of fetch granularity as required!\n", inint, device->readBufferSize);
				return(PsychError_invalidIntegerArg);
			}

			if (device->readerThread) {
				if (verbosity > 0) printf("Called StartBackgroundRead, but background read operations are already enabled! Disable first via 'StopBackgroundRead'!\n");
				return(PsychError_user);			
			}
			
			// Setup data structures:
			device->readerThreadWritePos = 0;
			device->clientThreadReadPos  = 0;
			device->readGranularity = inint;
			
			// Create & Init the mutex:
			if ((rc=pthread_mutex_init(&(device->readerLock), NULL))) {
				printf("PTB-ERROR: In StartBackgroundReadCould(): Could not create readerLock mutex lock [%s].\n", strerror(rc));
				return(PsychError_system);
			}
			
			// Create and startup thread:
			if ((rc=pthread_create(&(device->readerThread), NULL, PsychSerialUnixGlueReaderThreadMain, (void*) device))) {
				printf("PTB-ERROR: In StartBackgroundReadCould(): Could not create background reader thread [%s].\n", strerror(rc));
				return(PsychError_system);
			}
		}
	}
	
	// Stop a background reader?
	if ((p = strstr(configString, "StopBackgroundRead"))) {
		PsychIOOSShutdownSerialReaderThread(device);
	}
	
	// Done.
	return(PsychError_none);
}

/* PsychIOOSWriteSerialPort()
 *
 * Write data to serial port:
 * writedata = Ptr. to data buffer of uint8 values to write.
 * amount = Buffersize aka amount of bytes to write.
 * blocking = 0 --> Async, 1 --> Flush buffer and wait for write completion.
 * errmsg = Pointer to char array where error messages should be put to.
 * timestamp = Pointer to a 4 element vectorof double values where write-completion timestamps should be put to.
 * [0] = Final write completion timestamp.
 * [1] = Timestamp immediately before write() submission.
 * [2] = Timestamp immediately after write() submission.
 * [3] = Timestamp of last check for write completion.
 *
 * Returns number of bytes written, or -1 on error.
 */
int PsychIOOSWriteSerialPort(PsychSerialDeviceRecord* device, void* writedata, unsigned int amount, int blocking, char* errmsg, double* timestamp)
{
	int nwritten;
	unsigned long lsr = 0;  // Serial transmitter line status register.
	int outqueue_pending;	// Pending bytes in output queue.
	
	// Nonblocking mode?
	if (blocking <= 0) {
		// Yep. Set filedescriptor to non-blocking mode:
		// Set the O_NONBLOCK flag so subsequent I/O will not block.
		// See fcntl(2) ("man 2 fcntl") for details.
		if (fcntl(device->fileDescriptor, F_SETFL, O_NONBLOCK) == -1)
		{
			sprintf(errmsg, "Error setting O_NONBLOCK on device %s for non-blocking writes - %s(%d).\n", device->portSpec, strerror(errno), errno);
			return(-1);
		}
		
		// Write the data: Take pre- and postwrite timestamps.
		PsychGetAdjustedPrecisionTimerSeconds(&timestamp[1]);
		if ((nwritten = write(device->fileDescriptor, writedata, amount)) == -1)
		{
			sprintf(errmsg, "Error during write to device %s - %s(%d).\n", device->portSpec, strerror(errno), errno);
			return(-1);
		}
		PsychGetAdjustedPrecisionTimerSeconds(&timestamp[2]);

	}
	else {
		// Nope. Set filedescriptor to blocking mode:
		// Clear the O_NONBLOCK flag so subsequent I/O will block.
		// See fcntl(2) ("man 2 fcntl") for details.
		if (fcntl(device->fileDescriptor, F_SETFL, 0) == -1)
		{
			sprintf(errmsg, "Error clearing O_NONBLOCK on device %s for blocking writes - %s(%d).\n", device->portSpec, strerror(errno), errno);
			return(-1);
		}
		
		// Write the data: Take pre- and postwrite timestamps.
		PsychGetAdjustedPrecisionTimerSeconds(&timestamp[1]);
		if ((nwritten = write(device->fileDescriptor, writedata, amount)) == -1)
		{
			sprintf(errmsg, "Error during write to device %s - %s(%d).\n", device->portSpec, strerror(errno), errno);
			return(-1);
		}
		PsychGetAdjustedPrecisionTimerSeconds(&timestamp[2]);
		
		// Special polling mode to wait for transmit completion instead of tcdrain() + wait?
		if (blocking == 2) {
			// Yes. Use a tight polling loop which spin-waits until the driver output queue is empty (contains zero pending bytes):
			outqueue_pending = 1;
			while (outqueue_pending > 0) {
				// Take timestamp for this iteration:
				PsychGetAdjustedPrecisionTimerSeconds(&timestamp[3]);

				// Poll:
				ioctl(device->fileDescriptor, TIOCOUTQ, &outqueue_pending);
			}
		}
		else if ((PSYCH_SYSTEM == PSYCH_LINUX) && (blocking == 3)) {
			// Yes. Use a tight polling loop which spin-waits on the transmitter idle flag:
			// TODO: This may be not what we want, because the transmitter might be temporarily
			// idle although there is data pending in the higher-level write buffers. A more
			// proper combo might be to have blocking==2 polling for empty queue followed by
			// this call, so both conditions must be met.
			#if PSYCH_SYSTEM == PSYCH_LINUX
				// ioctl supported by device?
				if (ioctl(device->fileDescriptor, TIOCSERGETLSR, &lsr)==-1) {
					// Error! Can't use this method!
					sprintf(errmsg, "Error during write to device %s while trying to use blocking mode 3 to wait for write completion - %s(%d).\nMaybe function unsupported by device??\n", device->portSpec, strerror(errno), errno);
					return(-1);
				}
				
				// Seems to work. Go into the spin-waiting loop:
				while (!(lsr & TIOCSER_TEMT)) {
					// Take timestamp for this iteration:
					PsychGetAdjustedPrecisionTimerSeconds(&timestamp[3]);

					// Poll:
					ioctl(device->fileDescriptor, TIOCSERGETLSR, &lsr);
				}
			#endif
		}
		else {
			// Take timestamp for completeness although it doesn't make much sense in the blocking case:
			PsychGetAdjustedPrecisionTimerSeconds(&timestamp[3]);
			
			// Flush the write buffer and wait for write completion on physical hardware:
			if (tcdrain(device->fileDescriptor) == -1) {
				sprintf(errmsg, "Error during write to device %s while draining the write buffers - %s(%d).\n", device->portSpec, strerror(errno), errno);
				return(-1);
			}
		}
	}
	
	// Write successfully completed if we reach this point. Take timestamp, clear error message, return:
	PsychGetAdjustedPrecisionTimerSeconds(&timestamp[0]);
	errmsg[0] = 0;
	
	return(nwritten);
}

int PsychIOOSReadSerialPort(PsychSerialDeviceRecord* device, void** readdata, unsigned int amount, int blocking, char* errmsg, double* timestamp)
{
	struct termios	options;
	double timeout;
	int nread = 0;	
	*readdata = NULL;

	// Clamp 'amount' of data to be read to receive buffer size:
	if (amount > device->readBufferSize) {
		// Too much. Is amount unspecified aka INT_MAX? In that case,
		// we'll clamp it to readbuffers size. Otherwise we abort with
		// error.
		if (amount == INT_MAX) {
			// Clamp to buffersize:
			amount = device->readBufferSize;
		}
		else {
			// amount specified and impossible to satisfy request at
			// current readbuffer size. Abort with error and tell
			// user to resize readbuffer:
			sprintf(errmsg, "Amount of requested data %i is more than device %s can satisfy, as its input buffer is too small (%i bytes).\nSet a bigger readbuffer size please.\n", amount, device->portSpec, device->readBufferSize);
			return(-1);			
		}
		
	}
	
	// Async read active and amount > read granularity? If so, we clamp it to read granularity
	// to avoid ringbuffer wraparound problems.
	// TODO: FIXME -- Implement this ringbuffer wraparound properly to lift
	// this limitation!
	if ((device->readerThread) && (amount > device->readGranularity)) amount = device->readGranularity;
	
	// Nonblocking mode?
	if (blocking <= 0) {
		// Yep.
		
		// Background read active?
		if (device->readerThread) {
			// Just return what is available in the internal buffer:
			nread = PsychSerialUnixGlueAsyncReadbufferBytesAvailable(device);
		}
		else {
			// Set filedescriptor to non-blocking mode:
			// Set the O_NONBLOCK flag so subsequent I/O will not block.
			// See fcntl(2) ("man 2 fcntl") for details.
			if (fcntl(device->fileDescriptor, F_SETFL, O_NONBLOCK) == -1)
			{
				sprintf(errmsg, "Error setting O_NONBLOCK on device %s for non-blocking read - %s(%d).\n", device->portSpec, strerror(errno), errno);
				return(-1);
			}
			
			// Read the data, at most 'amount' bytes, nonblocking:
			if ((nread = read(device->fileDescriptor, device->readBuffer, amount)) == -1)
			{
				// Nothing to read? In nonblocking mode we simply return zero bytes read:
				if (errno == EAGAIN) return(0);
				
				// Some other error:
				sprintf(errmsg, "Error during non-blocking read from device %s - %s(%d).\n", device->portSpec, strerror(errno), errno);
				return(-1);
			}
		}
	}
	else {
		// Nope. This is a blocking read request:
		
		// Background read active?
		if (device->readerThread) {
			// Poll until requested amount of data is available:
			// TODO FIXME: Could do this in a more friendly way by using condition
			// variables, so we sleep until async reader thread signals availability
			// of sufficient data...
			PsychGetAdjustedPrecisionTimerSeconds(&timeout);
			*timestamp = timeout;
			timeout+=device->readTimeout;
			
			while((*timestamp < timeout) && (PsychSerialUnixGlueAsyncReadbufferBytesAvailable(device) < amount)) {
				PsychGetAdjustedPrecisionTimerSeconds(timestamp);
				PsychWaitIntervalSeconds(device->pollLatency);
			}
			
			// Return amount of available data:
			nread = PsychSerialUnixGlueAsyncReadbufferBytesAvailable(device);			
		}
		else {
			// Set filedescriptor to blocking mode:
			// Clear the O_NONBLOCK flag so subsequent I/O will block.
			// See fcntl(2) ("man 2 fcntl") for details.
			if (fcntl(device->fileDescriptor, F_SETFL, 0) == -1)
			{
				sprintf(errmsg, "Error clearing O_NONBLOCK on device %s for blocking read - %s(%d).\n", device->portSpec, strerror(errno), errno);
				return(-1);
			}
			
			// Retrieve current termios settings:
			if (tcgetattr(device->fileDescriptor, &options) == -1)
			{
				sprintf(errmsg, "Error getting current serial port device settings for device %s - %s(%d).\n", device->portSpec, strerror(errno), errno);
				return(-1);
			}
			
			// Change the number of bytes that must be received for blocking read()'s before read() returns.
			// read() will return if it can fetch at least that many bytes (we want 'amount' bytes at least),
			// or if the user specified timeout occurs (as set in Open or Configure routine):
			options.c_cc[VMIN] = amount;
			
			// Cause the new options to take effect immediately.
			if (tcsetattr(device->fileDescriptor, TCSANOW, &options) == -1)
			{
				sprintf(errmsg, "Error setting new serial port configuration attributes for device %s - %s(%d).\n", device->portSpec, strerror(errno), errno);
				return(-1);
			}
			
			// Need to poll for arrival of first byte, as the timeout timer [VTIME] is only armed after
			// reception of first byte:
			PsychGetAdjustedPrecisionTimerSeconds(&timeout);
			*timestamp = timeout;
			timeout+=device->readTimeout;
			
			while((*timestamp < timeout) && (PsychIOOSBytesAvailableSerialPort(device) < 1)) {
				PsychGetAdjustedPrecisionTimerSeconds(timestamp);
				PsychWaitIntervalSeconds(device->pollLatency);
			}
			
			if (PsychIOOSBytesAvailableSerialPort(device) < 1) {
				// Ok, first byte didn't arrive within one timeout period:
				return(0);
			}
			
			// Read the data, at most (and at least, unless timeout occurs) 'amount' bytes, blocking:
			if ((nread = read(device->fileDescriptor, device->readBuffer, amount)) == -1)
			{
				sprintf(errmsg, "Error during blocking read from device %s - %s(%d).\n", device->portSpec, strerror(errno), errno);
				return(-1);
			}		
			
			// Reset minbytes to zero:
			options.c_cc[VMIN] = 0;
			
			// Cause the new options to take effect immediately.
			if (tcsetattr(device->fileDescriptor, TCSANOW, &options) == -1)
			{
				sprintf(errmsg, "Error resetting new serial port configuration attributes for device %s - %s(%d).\n", device->portSpec, strerror(errno), errno);
				return(-1);
			}
		}
	}
	
	// Read successfully completed if we reach this point. Take timestamp, clear error message:
	PsychGetAdjustedPrecisionTimerSeconds(timestamp);
	errmsg[0] = 0;

	// Was this a fetch-op from an active background read?
	if (device->readerThread) {
		// Yes.

		// Check for buffer overflow:
		if (nread > device->readBufferSize) {
			sprintf(errmsg, "Error: Readbuffer overflow for background read operation on device %s for read. At least %i bytes have been lost!\n", device->portSpec, nread - device->readBufferSize);
			return(-1);
		}
		
		// Clamp available amount to requested (or maximum allowable) amount:
		nread = (nread > amount) ? amount : nread;
		
		// Copy current readpos to readdata:
		*readdata = (void*) &(device->readBuffer[(device->clientThreadReadPos) % (device->readBufferSize)]);

		// Update of read-pointer:
		device->clientThreadReadPos += nread;
	}
	else {
		// No. Standard read:
		*readdata = (void*) device->readBuffer;
	}
	
	return(nread);
}

int PsychIOOSBytesAvailableSerialPort(PsychSerialDeviceRecord* device)
{
	int navail = 0;
	
	if (device->readerThread) {
		// Async reader poll: Calculate what is available in the internal buffer:
		navail = PsychSerialUnixGlueAsyncReadbufferBytesAvailable(device);
	}
	else {
		// Regular poll: Perform query to system:
		if (ioctl(device->fileDescriptor, FIONREAD, &navail)!=0) {
			if (verbosity > 0) printf("Error during 'BytesAvailable': ioctl - FIONREAD byte available query on device %s returned %s(%d)\n", device->portSpec, strerror(errno), errno);
			return(-1);
		}
	}
	
	return(navail);
}

void PsychIOOSFlushSerialPort(PsychSerialDeviceRecord* device)
{	
	if (tcdrain(device->fileDescriptor)!=0) {
		if (verbosity > 0) printf("Error during 'Flush': tcdrain() on device %s returned %s(%d)\n", device->portSpec, strerror(errno), errno);
	}
	
	return;
}

void PsychIOOSPurgeSerialPort(PsychSerialDeviceRecord* device)
{
	if (tcflush(device->fileDescriptor, TCIOFLUSH)!=0) {
		if (verbosity > 0) printf("Error during 'Purge': tcflush(TCIFLUSH) on device %s returned %s(%d)\n", device->portSpec, strerror(errno), errno);
	}
	
	return;
}

#endif
