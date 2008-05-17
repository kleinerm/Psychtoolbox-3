/*
	IOPort.c		
    
	PROJECTS: 
	
		IOPort only.
  
	AUTHORS:
	
		Allen.Ingling@nyu.edu				awi
		mario.kleiner at tuebingen.mpg.de	mk
  
	PLATFORMS:	
	
		All.
    
	HISTORY:

		12/20/2004	awi		Wrote it.
		04/10/2008	mk		Started to extend/rewrite it to become a full-fledged generic I/O driver (serial port, parallel port, etc...).
 
	DESCRIPTION:
	
	
*/

#include "IOPort.h"

// Maximum number of slots aka allowed open ports:
#define	PSYCH_MAX_IOPORTS		100

// Level of verbosity:
int verbosity = 4;

// Bank of all port records:
static PsychPortIORecord	portRecordBank[PSYCH_MAX_IOPORTS];

// Total count of open ports:
static unsigned int		portRecordCount = 0;

// Array with function syntax strings:
#define MAX_SYNOPSIS_STRINGS 50  
static const char *synopsisSYNOPSIS[MAX_SYNOPSIS_STRINGS];

// Initialize usage info -- function overview:
void InitializeSynopsis(void)
{
	int i=0;
	const char **synopsis = synopsisSYNOPSIS;  //abbreviate the long name
	
	synopsis[i++] = "IOPort - A Psychtoolbox driver for general port I/O:\n";

	synopsis[i++] = "\nGeneral information:\n";
	synopsis[i++] = "version = IOPort('Version');";
	synopsis[i++] = "oldlevel = IOPort('Verbosity' [,level]);";

	synopsis[i++] = "\nGeneral commands for all types of input/output ports:\n";
	synopsis[i++] = "IOPort('Close', handle);";
	synopsis[i++] = "IOPort('CloseAll');";
	synopsis[i++] = "[nwritten, when, errmsg] = IOPort('Write', handle, data [, nonBlocking=0]);";
	synopsis[i++] = "IOPort('Flush', handle);"; 
	synopsis[i++] = "[data, when, errmsg] = IOPort('Read', handle [, nonBlocking=1] [, amount]);";
	synopsis[i++] = "navailable = IOPort('BytesAvailable', handle);";
	synopsis[i++] = "IOPort('Purge', handle);";

	synopsis[i++] = "\nCommands specific to serial ports:\n";
	synopsis[i++] = "handle = IOPort('OpenSerialPort', port [, configString]);";
	synopsis[i++] = "IOPort('ConfigureSerialPort', handle, configString);";

	synopsis[i++] = NULL;  //this tells IOPORTDisplaySynopsis where to stop
	if (i > MAX_SYNOPSIS_STRINGS) {
		PrintfExit("%s: increase dimension of synopsis[] from %ld to at least %ld and recompile.",__FILE__,(long)MAX_SYNOPSIS_STRINGS,(long)i);
	}
}

PsychError IOPORTVerbosity(void)
{
 	static char useString[] = "oldlevel = IOPort('Verbosity' [,level]);";
	static char synopsisString[] = 
		"Set level of verbosity for error/warning/status messages. 'level' optional, new level "
		"of verbosity. 'oldlevel' is the old level of verbosity. The following levels are "
		"supported: 0 = Shut up. 1 = Print errors, 2 = Print also warnings, 3 = Print also some info, "
		"4 = Print more useful info (default), >5 = Be very verbose (mostly for debugging the driver itself). ";		
	static char seeAlsoString[] = "";	 
	
	int level= -1;

	// Setup online help: 
	PsychPushHelp(useString, synopsisString, seeAlsoString);
	if(PsychIsGiveHelp()) {PsychGiveHelp(); return(PsychError_none); };
	
	PsychErrorExit(PsychCapNumInputArgs(1));     // The maximum number of inputs
	PsychErrorExit(PsychRequireNumInputArgs(0)); // The required number of inputs	
	PsychErrorExit(PsychCapNumOutputArgs(1));	 // The maximum number of outputs

	PsychCopyInIntegerArg(1, kPsychArgOptional, &level);
	if (level < -1) PsychErrorExitMsg(PsychError_user, "Invalid level of verbosity provided. Valid are levels of zero and greater.");
	
	// Return current/old level:
	PsychCopyOutDoubleArg(1, kPsychArgOptional, (double) verbosity);

	// Set new level, if one was provided:
	if (level > -1) verbosity = level;

	return(PsychError_none);
}

// Display usage info, aka synopsis if IOPort is called without any subfunction arguments:
PsychError IOPORTDisplaySynopsis(void)
{
	int i;
	
	for (i = 0; synopsisSYNOPSIS[i] != NULL; i++) printf("%s\n",synopsisSYNOPSIS[i]);
	
	return(PsychError_none);
}

// Exit function: Called at shutdown time:
PsychError PsychExitIOPort(void)
{
	int i;
	
	// Close all open ports:
	for (i=0; i < PSYCH_MAX_IOPORTS; i++) {
		if (portRecordBank[i].portType) PsychCloseIOPort(i);
	}

    return(PsychError_none);	
}

// Init function: Called at startup time:
PsychError PsychInitIOPort(void)
{
	int i;
	
	// Init array of IOPort records to empty:
	for (i=0; i < PSYCH_MAX_IOPORTS; i++) {
		portRecordBank[i].portType = KPsychIOPortNone;
	}
	
	// No ports open yet:
	portRecordCount = 0;
	
    return(PsychError_none);
}

// Receive pointer to PsychPortIORecord for given userspace port handle:
PsychPortIORecord* PsychGetPortIORecord(int handle)
{
	char errmsg[1000];
	errmsg[0] = 0;
	
	if (handle < 0 || handle >= PSYCH_MAX_IOPORTS || portRecordBank[handle].portType == KPsychIOPortNone) {
		sprintf(errmsg, "Invalid port handle %i provided. No such port open. Maybe you closed it beforehand?", handle);
		PsychErrorExitMsg(PsychError_user, errmsg);
	}
	
	// Valid handle, referring to open port: Return its data structure...
	return(&portRecordBank[handle]);
}

// Close port referenced by 'handle':
PsychError PsychCloseIOPort(int handle)
{
	PsychPortIORecord*	portRecord;
	char errmsg[1000];
	errmsg[0] = 0;

	// Retrieve and assign open port for handle: Will check for invalid handles and closed ports...
	portRecord = PsychGetPortIORecord(handle);
	
	switch(portRecord->portType) {
		case kPsychIOPortSerial:
			// Close serial port:
			PsychIOOSCloseSerialPort(portRecord->device);
		break;
		
		default:
			PsychErrorExitMsg(PsychError_internal, "Unknown portType - Unsupported.");
	}

	// Reset port to "no device" aka closed:
	portRecord->device = NULL;
	portRecord->portType = KPsychIOPortNone;
	
	// Decrement count of open ports:
	portRecordCount--;
	
	return(PsychError_none);
}

// Write data to port:
int PsychWriteIOPort(int handle, void* writedata, unsigned int amount, int nonblocking, char* errmsg, double* timestamp)
{
	PsychPortIORecord*	portRecord = PsychGetPortIORecord(handle);
	
	switch(portRecord->portType) {
		case kPsychIOPortSerial:
			// Write to serial port:
			return(PsychIOOSWriteSerialPort(portRecord->device, writedata, amount, nonblocking, errmsg, timestamp));
		break;
		
		default:
			PsychErrorExitMsg(PsychError_internal, "Unknown portType - Unsupported.");
	}	

	// Not reached, just to make compiler happy:
	return(0);
}

int	PsychReadIOPort(int handle, void** readbuffer, unsigned int amount, int nonblocking, char* errmsg, double* timestamp)
{
	PsychPortIORecord*	portRecord = PsychGetPortIORecord(handle);
	
	switch(portRecord->portType) {
		case kPsychIOPortSerial:
			// Read from serial port:
			return(PsychIOOSReadSerialPort(portRecord->device, readbuffer, amount, nonblocking, errmsg, timestamp));
		break;
		
		default:
			PsychErrorExitMsg(PsychError_internal, "Unknown portType - Unsupported.");
	}	

	// Not reached, just to make compiler happy:
	return(0);
}

int PsychBytesAvailableIOPort(int handle)
{
	PsychPortIORecord*	portRecord = PsychGetPortIORecord(handle);
	
	switch(portRecord->portType) {
		case kPsychIOPortSerial:
			// Read from serial port:
			return(PsychIOOSBytesAvailableSerialPort(portRecord->device));
		break;
		
		default:
			PsychErrorExitMsg(PsychError_internal, "Unknown portType - Unsupported.");
	}	

	// Not reached, just to make compiler happy:
	return(0);
}

void PsychPurgeIOPort(int handle)
{
	PsychPortIORecord*	portRecord = PsychGetPortIORecord(handle);
	
	switch(portRecord->portType) {
		case kPsychIOPortSerial:
			// Purge serial port:
			PsychIOOSPurgeSerialPort(portRecord->device);
		break;
		
		default:
			PsychErrorExitMsg(PsychError_internal, "Unknown portType - Unsupported.");
	}	
}

void PsychFlushIOPort(int handle)
{
	PsychPortIORecord*	portRecord = PsychGetPortIORecord(handle);
	
	switch(portRecord->portType) {
		case kPsychIOPortSerial:
			// Purge serial port:
			PsychIOOSFlushSerialPort(portRecord->device);
		break;
		
		default:
			PsychErrorExitMsg(PsychError_internal, "Unknown portType - Unsupported.");
	}
}


PsychError IOPORTIOPort(void)
{
    return(PsychError_none);	
}

// Close given port.
PsychError IOPORTClose(void)
{
 	static char useString[] = "IOPort('Close', handle);";
	static char synopsisString[] = 
		"Close a I/O port device 'handle'. This works for all device types.\n";
	static char seeAlsoString[] = "'CloseAll'";	 
  	
	int handle;
	
	// Setup online help: 
	PsychPushHelp(useString, synopsisString, seeAlsoString);
	if(PsychIsGiveHelp()) {PsychGiveHelp(); return(PsychError_none); };
	
	PsychErrorExit(PsychCapNumInputArgs(1));     // The maximum number of inputs
	PsychErrorExit(PsychRequireNumInputArgs(1)); // The required number of inputs	
	PsychErrorExit(PsychCapNumOutputArgs(0));	 // The maximum number of outputs

	// Get handle:
	PsychCopyInIntegerArg(1, kPsychArgRequired, &handle);

	// Close associated port:
	PsychCloseIOPort(handle);
	
    return(PsychError_none);	
}

// Close all ports of all devices -- Effectively a shutdown.
PsychError IOPORTCloseAll(void)
{
 	static char useString[] = "IOPort('CloseAll');";
	static char synopsisString[] = 
		"Close all open I/O port devices, regardless of type. Release all ressources.\n";
	static char seeAlsoString[] = "'Close'";	 
  	
	// Setup online help: 
	PsychPushHelp(useString, synopsisString, seeAlsoString);
	if(PsychIsGiveHelp()) {PsychGiveHelp(); return(PsychError_none); };
	
	PsychErrorExit(PsychCapNumInputArgs(0));     // The maximum number of inputs
	PsychErrorExit(PsychRequireNumInputArgs(0)); // The required number of inputs	
	PsychErrorExit(PsychCapNumOutputArgs(0));	 // The maximum number of outputs

	// Close all ports by calling our shutdown function:
	PsychExitIOPort();
	
    return(PsychError_none);	
}

// Open a serial port on a serial port device:
PsychError IOPORTOpenSerialPort(void)
{
 	static char useString[] = "handle = IOPort('OpenSerialPort', port [, configString]);";
	static char synopsisString[] = 
		"Open a serial port device, return a 'handle' to it.\n"
		"'port' is usually a name string that defines the serial port device "
		"to open. On MS-Windows this could be, e.g., 'COM1' or 'COM2' etc. On "
		"Apple OS/X, it is the path to a BSD device file, e.g., '/dev/cu.usbserial-FT3Z95V5' "
		"for a serial-over-USB device with unique id FT3Z95V5. On GNU/Linux it could be "
		"'/dev/ttyS0' for the first real serial port, or '/dev/ttyUSB0' for the first "
		"serial-over-USB device.\n\n"
		"The optional string 'configString' is a string with pairs of paramName=paramValue "
		"tokens, separated by a delimiter, e.g., a space. It allows to specify specific "
		"values 'paramValue' to specific serial port parameters 'paramName'. Not all "
		"parameters are supported by all operating systems, and all settings have reasonable "
		"defaults. Settings unknown to a specific operating system are ignored.\n"
		"The following is a list of (possibly) supported parameters with their defaults:\n\n"
		"BaudRate=9600 -- The baud transmission rate of the connection. Standard baud rates include "
		"110, 300, 600, 1200, 2400, 4800, 9600, 14400, 19200, 38400, 57600, 115200, 128000 and "
		"256000 bits per second. Not all values may be supported by all operating systems and drivers.\n\n"
		"Parity=None   -- Type of parity checking: None, Even, Odd.\n\n"
		"DataBits=8    -- Number of data bits per packet: 5,6,7 or 8, on Windows also 16.\n\n"
		"StopBits=1    -- Number of stop bits per packet: 1 or 2.\n\n"
		"FlowControl=None  -- Type of flow control: None, Hardware (RTS/CTS lines), Software (XON/XOFF characters).\n\n"
		"Terminator=os default  -- Type of terminator, given as ASCII character value, e.g., 13 for char(13) aka CR. Currently unused\n\n"
		"DTR=os default	-- Setting for 'Data Terminal Ready' pin: 0 or 1.\n\n"
		"RTS=os default	-- Setting for 'Request To Send' pin: 0 or 1.\n\n"
		"BreakBehaviour=Ignore -- Behaviour if a 'Break Condition' is detected on the line: Ignore, Flush, Zero. On Windows, this setting is ignored.\n\n"
		"OutputBufferSize=4096 -- Size of output buffer in bytes.\n\n"
		"InputBufferSize=4096 -- Size of input buffer in bytes. You can't read more than that amount per read command.\n\n"
		"The following timeout values are inter-byte timeouts. You specify how much time reception or "
		"transmission of a single byte is allowed to take. Timeout occurs if more than that time elapses "
		"between send/reception of two consecutive bytes or if the total amount of time exceeds the "
		"number of bytes, times the interbyte timeout value. A value of zero means not to use any timeout, "
		"in which case a blocking read or write may take forever. If a timeout occurs, the read or write "
		"operation will be aborted. \n"
		"Granularity of timeout settings is 100 msecs on OS/X and Linux, 1 msec on Windows, all values "
		"are rounded to the closest value matching that granularity. The minimal timeout is 100 msecs "
		"on OS/X and Linux, about 6 msecs on Windows.\n\n"
		"SendTimeout=1.0 -- Interbyte send timeout in seconds. Only used on Windows.\n\n"
		"ReceiveTimeout=1.0 -- Interbyte receive timeout in seconds.\n\n"
		"ReceiveLatency=0.000001 -- Latency in seconds for processing of new input bytes. Only used on OS/X.\n\n"
		"ProcessingMode=Raw -- Mode of input/output processing: Raw or Cooked. On Windows, only Raw (binary) mode is supported.\n\n";
		
//	Input, probably not settable:	"DSR=os default	-- Setting for 'Data Set Ready' pin: 0 or 1.\n\n"
//	Input, probably not settable:	"CTS=os default	-- Setting for 'Clear To Send' pin: 0 or 1.\n\n"

	static char seeAlsoString[] = "'CloseAll'";	 
  	
	static char defaultConfig[] = "BaudRate=9600 Parity=None DataBits=8 StopBits=1 FlowControl=None ReceiveLatency=0.000001 SendTimeout=1.0 ReceiveTimeout=1.0 ProcessingMode=Raw BreakBehaviour=Ignore OutputBufferSize=4096 InputBufferSize=4096"; 
	char		finalConfig[2000];
	char*		portSpec = NULL;
	char*		configString = NULL;
	PsychSerialDeviceRecord* device = NULL;
	int			handle;
	
	// Setup online help: 
	PsychPushHelp(useString, synopsisString, seeAlsoString);
	if(PsychIsGiveHelp()) {PsychGiveHelp(); return(PsychError_none); };
	
	PsychErrorExit(PsychCapNumInputArgs(2));     // The maximum number of inputs
	PsychErrorExit(PsychRequireNumInputArgs(1)); // The required number of inputs	
	PsychErrorExit(PsychCapNumOutputArgs(1));	 // The maximum number of outputs

	// Get required portSpec:
	PsychAllocInCharArg(1, kPsychArgRequired, &portSpec);
	
	// Get the optional configString:
	if (!PsychAllocInCharArg(2, kPsychArgOptional, &configString)) {
		// No config string specified: Assign default string:
		sprintf(finalConfig, "%s", defaultConfig);
	}
	else {
		// Config string provided: Prepend it to default string. That way, all settings
		// spec'd by usercode will override the defaultConfig, but non-specified settings
		// will be provided by defaultConfig:
		sprintf(finalConfig, "%s %s", configString, defaultConfig);		
	}
	
	// Search for a free slot:
	if (portRecordCount >= PSYCH_MAX_IOPORTS) PsychErrorExitMsg(PsychError_user, "Maximum number of open Input/Output ports exceeded.");
	
	// Iterate until end or free slot:
	for (handle=0; (handle < PSYCH_MAX_IOPORTS) && (portRecordBank[handle].portType); handle++);
	if (portRecordBank[handle].portType) PsychErrorExitMsg(PsychError_user, "Maximum number of open Input/Output ports exceeded.");
	// handle is index into our port record...

	// Call OS specific open routine for serial port:
	device = PsychIOOSOpenSerialPort(portSpec, finalConfig);
	
	// If we reach this point, then device is the pointer to the class specific device struct and the
	// open operation was successfull. Build port struct:
	portRecordBank[handle].portType = kPsychIOPortSerial;
	portRecordBank[handle].device = (void*) device;
	portRecordCount++;
	
	// Return handle to new serial port object:
	PsychCopyOutDoubleArg(1, kPsychArgRequired, (double) handle);
	
    return(PsychError_none);
}

// Open a serial port on a serial port device:
PsychError IOPORTConfigureSerialPort(void)
{
 	static char useString[] = "IOPort('ConfigureSerialPort', handle, configString);";
	static char synopsisString[] = 
		"(Re-)Configure a serial port device, specified by 'handle'.\n"
		"The string 'configString' is a string with pairs of paramName=paramValue "
		"tokens, separated by a delimiter, e.g., a space. It allows to specify specific "
		"values 'paramValue' to specific serial port parameters 'paramName'. Not all "
		"parameters are supported by all operating systems, and all settings have reasonable "
		"defaults. Settings unknown to a specific operating system are ignored.\n\n"
		"See the help of 'OpenSerialPort' for possible settings.";
		
	static char seeAlsoString[] = "'OpenSerialPort'";
	char* configString = NULL;
	PsychSerialDeviceRecord* device = NULL;
	int handle;
	
	// Setup online help: 
	PsychPushHelp(useString, synopsisString, seeAlsoString);
	if(PsychIsGiveHelp()) {PsychGiveHelp(); return(PsychError_none); };
	
	PsychErrorExit(PsychCapNumInputArgs(2));     // The maximum number of inputs
	PsychErrorExit(PsychRequireNumInputArgs(2)); // The required number of inputs	
	PsychErrorExit(PsychCapNumOutputArgs(0));	 // The maximum number of outputs

	// Get required portSpec:
	PsychCopyInIntegerArg(1, kPsychArgRequired, &handle);
	
	// Get the configString:
	PsychAllocInCharArg(2, kPsychArgRequired, &configString);
	
	// Return return value of configuration call:
	return(PsychIOOSConfigureSerialPort(PsychGetPortIORecord(handle)->device, configString));
}

PsychError IOPORTRead(void)
{
 	static char useString[] = "[data, when, errmsg] = IOPort('Read', handle [, nonBlocking=1] [, amount]);";
	static char synopsisString[] = 
		"Read data from device, specified by 'handle'.\n"
		"Returned 'data' will be a row vector of read data bytes. 'when' will be a receive "
		"timestamp of when the data read was complete. 'errmsg' will be a human readable "
		"char string with an error message if any error occured, otherwise an empty string. "
		"The optional flag 'nonBlocking' if set to 1 will ask the read function to not block, "
		"but return immediately: The read function will return whatever amount of data is "
		"currently available in the internal input queue, but at most 'amount' bytes if 'amount' "
		"is specified. If no data is available, it will return an empty matrix. This is the default.\n"
		"If nonBlocking is set to 0, you must specify the 'amount' of bytes to receive and the "
		"function will wait until that exact amount of data is available, then return it."
		"Even in blocking mode, the function will return if no data becomes available within "
		"the time period specified by the configuration setting 'ReadTimeout' (see help for "
		"'OpenSerialPort' for description). In some situations, the read function may return "
		"less data than requested, e.g., in specific error cases, where a read could only "
		"get partially satisfied.";
		
	static char seeAlsoString[] = "'Write', 'OpenSerialPort', 'ConfigureSerialPort'";
	
	char			errmsg[1024];
	int				handle, nonblocking, nread, amount, i;
	psych_uint8*	readbuffer;
	double*			outbuffer;
	double			timestamp;	
	errmsg[0] = 0;
	
	// Setup online help: 
	PsychPushHelp(useString, synopsisString, seeAlsoString);
	if(PsychIsGiveHelp()) {PsychGiveHelp(); return(PsychError_none); };
	
	PsychErrorExit(PsychCapNumInputArgs(3));     // The maximum number of inputs
	PsychErrorExit(PsychRequireNumInputArgs(1)); // The required number of inputs	
	PsychErrorExit(PsychCapNumOutputArgs(3));	 // The maximum number of outputs

	// Get required port handle:
	PsychCopyInIntegerArg(1, kPsychArgRequired, &handle);
	
	// Get optional nonblocking flag: Defaults to 1 -- non-blocking.
	nonblocking = 1;
	PsychCopyInIntegerArg(2, kPsychArgOptional, &nonblocking);

	// Get optional maximum or exact amount to read:
	amount = INT_MAX;
	if (!PsychCopyInIntegerArg(3, kPsychArgOptional, &amount)) {
		// Not spec'd:
		if (nonblocking <= 0) PsychErrorExitMsg(PsychError_user, "When issuing a 'Read' in blocking mode, you must specify the exact 'amount' to read, but 'amount' was omitted!");
	}
	
	if (amount < 0) PsychErrorExitMsg(PsychError_user, "Invalid (negative) 'amount' of data to read!");
	
	// Read data:
	nread = PsychReadIOPort(handle, (void**) &readbuffer, amount, nonblocking, errmsg, &timestamp);
	
	// Allocate outbuffer of proper size:
	PsychAllocOutDoubleMatArg(1, kPsychArgOptional, 1, ((nread >=0) ? nread : 0), 1, &outbuffer);

	// Copy to outbuffer: We copy our uint8 values to a double matrix. This is arguably a bit of a waste of
	// memory and bandwidth, but it simplifies interfacing with Octave, old Matlab versions and possible
	// future runtime environments a lot:
	if (nread > 0) for (i=0; i<nread; i++) outbuffer[i] = readbuffer[i];
	
	if (nread < 0 && verbosity > 0) printf("IOPort: Error: %s\n", errmsg); 

	// Return timestamp and errmsg, if any:
	PsychCopyOutDoubleArg(2, kPsychArgOptional, timestamp);
	PsychCopyOutCharArg(3, kPsychArgOptional, errmsg);
	
    return(PsychError_none);
}

PsychError IOPORTWrite(void)
{
 	static char useString[] = "[nwritten, when, errmsg] = IOPort('Write', handle, data [, nonBlocking=0]);";
	static char synopsisString[] = 
		"Write data to device, specified by 'handle'.\n"
		"'data' must be a vector of data items to write, or a matrix (in which case data "
		"in the matrix will be transmitted in column-major order, ie., first the first "
		"column, then the 2nd column etc...), either with data elements of uint8 class "
		"or a (1 Byte per char) character string. The optional flag 'nonBlocking' if "
		"set to 1 will ask the write function to not block, but return immediately, ie. "
		"data is sent/written in the background while your code continues to execute - There "
		"may be an arbitrary delay until data transmission is really finished. The default "
		"setting is blocking writes - The function waits until data transmission is really "
		"finished.\n\n"
		"Optionally, the function returns the following return arguments:\n"
		"'nwritten' Number of bytes written -- Should match amount of data provided on success.\n"
		"'when' A timestamp of write completion: This is only meaningful in blocking mode!\n"
		"'errmsg' A system defined error message if something wen't wrong. ";

	static char seeAlsoString[] = "";
	
	char			errmsg[1024];
	int				handle, nonblocking, m, n, p, nwritten;
	psych_uint8*	inData = NULL;
	char*			inChars = NULL;
	void*			writedata = NULL;
	double			timestamp;
	errmsg[0] = 0;

	// Setup online help: 
	PsychPushHelp(useString, synopsisString, seeAlsoString);
	if(PsychIsGiveHelp()) {PsychGiveHelp(); return(PsychError_none); };
	
	PsychErrorExit(PsychCapNumInputArgs(3));     // The maximum number of inputs
	PsychErrorExit(PsychRequireNumInputArgs(2)); // The required number of inputs	
	PsychErrorExit(PsychCapNumOutputArgs(3));	 // The maximum number of outputs

	// Get required port handle:
	PsychCopyInIntegerArg(1, kPsychArgRequired, &handle);

	// Get the data:
	switch(PsychGetArgType(2)) {
		case PsychArgType_uint8:
			PsychAllocInUnsignedByteMatArg(2, kPsychArgRequired, &m, &n, &p, &inData);
			if (p!=1 || m * n == 0) PsychErrorExitMsg(PsychError_user, "'data' is not a vector or 2D matrix, but some higher dimensional matrix!");
			n = m * n;
			writedata = (void*) inData;
		break;
		
		case PsychArgType_char:
			PsychAllocInCharArg(2, kPsychArgRequired, &inChars);
			n = strlen(inChars);
			writedata = (void*) inChars;
		break;
		
		default:
			PsychErrorExitMsg(PsychError_user, "Invalid type for 'data' vector: Must be an uint8 or char vector.");
	}
	
	
	// Get optional nonblocking flag: Defaults to zero -- blocking.
	nonblocking = 0;
	PsychCopyInIntegerArg(3, kPsychArgOptional, &nonblocking);

	// Write data:
	nwritten = PsychWriteIOPort(handle, writedata, n, nonblocking, errmsg, &timestamp);
	if (nwritten < 0 && verbosity > 0) printf("IOPort: Error: %s\n", errmsg); 
	
	PsychCopyOutDoubleArg(1, kPsychArgOptional, nwritten);
	PsychCopyOutDoubleArg(2, kPsychArgOptional, timestamp);
	PsychCopyOutCharArg(3, kPsychArgOptional, errmsg);
	
    return(PsychError_none);
}

PsychError IOPORTBytesAvailable(void)
{
 	static char useString[] = "navailable = IOPort('BytesAvailable', handle);";
	static char synopsisString[] = "Return number 'navailable' of data bytes available for readout from device specified by 'handle'.";
	static char seeAlsoString[] = "'Read'";
	
	int				handle;
		
	// Setup online help: 
	PsychPushHelp(useString, synopsisString, seeAlsoString);
	if(PsychIsGiveHelp()) {PsychGiveHelp(); return(PsychError_none); };
	
	PsychErrorExit(PsychCapNumInputArgs(1));     // The maximum number of inputs
	PsychErrorExit(PsychRequireNumInputArgs(1)); // The required number of inputs	
	PsychErrorExit(PsychCapNumOutputArgs(1));	 // The maximum number of outputs

	// Get required port handle:
	PsychCopyInIntegerArg(1, kPsychArgRequired, &handle);
	
	// Copy out number of bytes available for read on port:
	PsychCopyOutDoubleArg(1, kPsychArgOptional, (double) PsychBytesAvailableIOPort(handle));

    return(PsychError_none);
}

PsychError IOPORTPurge(void)
{
 	static char useString[] = "IOPort('Purge', handle);";
	static char synopsisString[] = "Purge all data queued for readin from device specified by 'handle'. All unread data is discarded.";
	static char seeAlsoString[] = "'Flush'";
	
	int				handle;
		
	// Setup online help: 
	PsychPushHelp(useString, synopsisString, seeAlsoString);
	if(PsychIsGiveHelp()) {PsychGiveHelp(); return(PsychError_none); };
	
	PsychErrorExit(PsychCapNumInputArgs(1));     // The maximum number of inputs
	PsychErrorExit(PsychRequireNumInputArgs(1)); // The required number of inputs	
	PsychErrorExit(PsychCapNumOutputArgs(1));	 // The maximum number of outputs

	// Get required port handle:
	PsychCopyInIntegerArg(1, kPsychArgRequired, &handle);
	
	// Do it:
	PsychPurgeIOPort(handle);

    return(PsychError_none);
}

PsychError IOPORTFlush(void)
{
 	static char useString[] = "IOPort('Flush', handle);";
	static char synopsisString[] = "Flush all data queued for writeout to device specified by 'handle', wait for full write completion.";
	static char seeAlsoString[] = "'Flush'";
	
	int				handle;
		
	// Setup online help: 
	PsychPushHelp(useString, synopsisString, seeAlsoString);
	if(PsychIsGiveHelp()) {PsychGiveHelp(); return(PsychError_none); };
	
	PsychErrorExit(PsychCapNumInputArgs(1));     // The maximum number of inputs
	PsychErrorExit(PsychRequireNumInputArgs(1)); // The required number of inputs	
	PsychErrorExit(PsychCapNumOutputArgs(1));	 // The maximum number of outputs

	// Get required port handle:
	PsychCopyInIntegerArg(1, kPsychArgRequired, &handle);
	
	// Do it:
	PsychFlushIOPort(handle);

    return(PsychError_none);
}
