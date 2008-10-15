/*
    File:			serialPort.c
	
    Description:	This routine provides a serial port API. It is heavily based on
					Apple's SerialPortSample example found at http://developer.apple.com/

	$Id: serialPort.c,v 1.1.1.1 2004/05/18 15:02:25 tom Exp $
	
*/

#include <fcntl.h>
#include <sys/ioctl.h>
#include <paths.h>
#include <sysexits.h>
#include <sys/select.h>
#include <sys/time.h>
#include <time.h>

#include <CoreFoundation/CoreFoundation.h>

#include <IOKit/IOKitLib.h>
#include <IOKit/serial/IOSerialKeys.h>
#include <IOKit/IOBSD.h>
#include "serialPort.h"



int portStatus( PORTINFO *info )
{
    int				handshake;
	int				i;
	struct termios  options;

    printf("BSD path: '%s'\n", info->bsdPath );
    printf("file descriptor = %d\n", info->fd );
    printf("buffer size = %d\n", info->str_cnt );
    printf("EOL = %d\n", info->eol );
	

    // Get the current options and save them so we can restore the default settings later.
	//
    if (tcgetattr( info->fd, &options ) == -1)
    {
        printf("Error getting tty attributes - %s(%d).\n",
            strerror(errno), errno);
        return -1;
    }

    // Print the current input and output baud rates.
    // See tcsetattr(4) ("man 4 tcsetattr") for details.
    //
    printf("Input/output baud rate is %d / %d\n", 
			(int) cfgetispeed(&options), (int) cfgetospeed(&options) );

    
	//	tcflag_t	c_iflag;	/* input flags */
	//	tcflag_t	c_oflag;	/* output flags */
	//	tcflag_t	c_cflag;	/* control flags */
	//	tcflag_t	c_lflag;	/* local flags */
	//	cc_t		c_cc[NCCS];	/* control chars */
	//	speed_t		c_ispeed;	/* input speed */
	//	speed_t		c_ospeed;	/* output speed */

	printf( "input flags:   c_iflag = 0x%08lx\n", options.c_iflag );
	printf( "output flags:  c_oflag = 0x%08lx\n", options.c_oflag );
	printf( "control flags: c_cflag = 0x%08lx\n", options.c_cflag );
	printf( "local flags:   c_iflag = 0x%08lx\n", options.c_iflag );

	printf( "control chars:    c_cc = " );
	for( i=0; i<NCCS/2; i++ ) {
		printf( "%02x ", options.c_cc[i] );
	}
	printf( "\n                         " );
	for( i=NCCS/2; i<NCCS; i++ ) {
		printf( "%02x ", options.c_cc[i] );
	}
	printf( "\n" );
	
    // To read the state of the modem lines, use the following ioctl.
    // See tty(4) ("man 4 tty") and ioctl(2) ("man 2 ioctl") for details.
    //
   if (ioctl(info->fd, TIOCMGET, &handshake) == -1)
    {
        printf("Error getting handshake lines - %s(%d).\n",
            strerror(errno), errno);    }

	//  TIOCM_LE	0001		/* line enable */
	//  TIOCM_DTR	0002		/* data terminal ready */
	//  TIOCM_RTS	0004		/* request to send */
	//  TIOCM_ST	0010		/* secondary transmit */
	//  TIOCM_SR	0020		/* secondary receive */
	//  TIOCM_CTS	0040		/* clear to send */
	//  TIOCM_CAR	0100		/* carrier detect */
	//  TIOCM_RNG	0200		/* ring */
	//  TIOCM_DSR	0400		/* data set ready */
	
    printf("Handshake lines currently set to 0x%04x\n", handshake);
	
	return 0;
}



int sendBreak( PORTINFO *info )
{
    if (ioctl(info->fd, TIOCSBRK, NULL ) == -1)
    {
        printf("Error setting break - %s(%d).\n",
            strerror(errno), errno);
		return -1;
    }
	
	usleep( 50000 );			// is this too long or too short ?
	
    if (ioctl(info->fd, TIOCCBRK, NULL ) == -1)
    {
        printf("Error clearing break - %s(%d).\n",
            strerror(errno), errno);
		return -1;
    }
    return 0;
}



int flushPort( PORTINFO *info )
{
	int		what = 0;
	
    if (ioctl(info->fd, TIOCFLUSH, &what) == -1)
    {
        printf("Error flushing input and output queues - %s(%d).\n",
            strerror(errno), errno);
		return -1;
    }
    return 0;
}




int setHandshake( PORTINFO *info, char *string )
{
	int				i;
	struct termios  options;
	

    // Get the current options and save them so we can restore the default settings later.
	//
    if (tcgetattr( info->fd, &options ) == -1)
    {
        printf("Error getting tty attributes - %s(%d).\n",
            strerror(errno), errno);
        return -1;
    }

	options.c_cflag &= ~(
			CCTS_OFLOW |			// clear CTS flow control of output
			CRTS_IFLOW );			// clear RTS flow control of input
	options.c_iflag &= ~(
			IXON |					// disable output flow control
			IXOFF );				// disable input flow control
	
	for( i=0; i<strlen(string); i++ )
	{
		switch( string[i] ) {
			case 's':
				options.c_iflag |= (
						IXON |					// enable output flow control
						IXOFF );				// enable input flow control
				break;
				
			case 'h':
				options.c_cflag |= (
						CCTS_OFLOW |			// set CTS flow control of output
						CRTS_IFLOW );			// set RTS flow control of input
				break;
				
			case 'n':
				break;
				
			default:
				printf( "Bad handshake specification.\n" );
				return -1;
		}
	}	
	
    if (tcsetattr(info->fd, TCSANOW, &options) == -1)
    {
        printf("Error setting tty attributes - %s(%d).\n",
            strerror(errno), errno);
        return -1;
    }
	
	return 0;
}


// Decodes a serial port configuration string and set the
// termios data structure accordingly.
// The string is in standard DOS format: 
//		"baud rate, parity, #data bits, #stop bits". 
//
int decodeConfigStr( struct termios *options, char *string )
{
	int		speed;
	char	parity;
	int		bits;
	int		stop;
	
	if( sscanf( string, "%d, %1c,%1d,%1d", &speed, &parity, &bits, &stop ) != 4 ) {
		printf( "Less than four items found in configuration string.\n" );
		return -1;
	}
	
    
    // Set raw input (non-canonical) mode, with reads blocking until either a single character 
    // has been received or a one second timeout expires.
    // See tcsetattr(4) ("man 4 tcsetattr") and termios(4) ("man 4 termios") for details.
    
    cfmakeraw(options);
    options->c_cc[VMIN] = 0;
    options->c_cc[VTIME] = 0;
        
    // The baud rate, word length, and handshake options can be set as follows:
    
    if( cfsetspeed( options, speed ) ) {		// Set baud rate 
		printf( "Error setting speed - %s(%d).\n",
            strerror(errno), errno);
		return -1;
	}
	  
    options->c_cflag = (
			CCTS_OFLOW |				// CTS flow control of output
			CRTS_IFLOW);				// RTS flow control of input
			
	switch( parity ) {
		case 'o':   options->c_cflag |= PARODD;
		case 'e':   options->c_cflag |= PARENB;
		case 'n':   break;
		default:
			printf( "Bad parity specification.\n" );
			return -1;
	}
	
	switch( bits ) {
		case 8:   options->c_cflag |= CS8; break;
		case 7:   options->c_cflag |= CS7; break;
		case 6:   options->c_cflag |= CS6; break;
		case 5:   options->c_cflag |= CS5; break;
		default:
			printf( "Bad word size specification.\n" );
			return -1;
	}
	
	switch( stop ) {
		case 2:   options->c_cflag |= CSTOPB;
		case 1:   break;
		default:
			printf( "Bad stop bits specification.\n" );
			return -1;
	}
	    
	return 0;
}


// Returns an iterator across all known modems. Caller is responsible for
// releasing the iterator when iteration is complete.
//
static kern_return_t FindModems(io_iterator_t *matchingServices)
{
    kern_return_t		kernResult; 
    mach_port_t			masterPort;
    CFMutableDictionaryRef	classesToMatch;

/*! @function IOMasterPort
    @abstract Returns the mach port used to initiate communication with IOKit.
    @discussion Functions that don't specify an existing object require the IOKit master port to be passed. This function obtains that port.
    @param bootstrapPort Pass MACH_PORT_NULL for the default.
    @param masterPort The master port is returned.
    @result A kern_return_t error code. */

    kernResult = IOMasterPort(MACH_PORT_NULL, &masterPort);
    if (KERN_SUCCESS != kernResult)
    {
        printf("IOMasterPort returned %d.\n", kernResult );
		goto exit;
    }
        
/*! @function IOServiceMatching
    @abstract Create a matching dictionary that specifies an IOService class match.
    @discussion A very common matching criteria for IOService is based on its class. IOServiceMatching will create a matching dictionary that specifies any IOService of a class, or its subclasses. The class is specified by C-string name.
    @param name The class name, as a const C-string. Class matching is successful on IOService's of this class or any subclass.
    @result The matching dictionary created, is returned on success, or zero on failure. The dictionary is commonly passed to IOServiceGetMatchingServices or IOServiceAddNotification which will consume a reference, otherwise it should be released with CFRelease by the caller. */

    // Serial devices are instances of class IOSerialBSDClient
    classesToMatch = IOServiceMatching(kIOSerialBSDServiceValue);
    if (classesToMatch == NULL)
    {
        printf("IOServiceMatching returned a NULL dictionary.\n");
    }
    else {
/*!
	@function CFDictionarySetValue
	Sets the value of the key in the dictionary.
	@param theDict The dictionary to which the value is to be set. If this
		parameter is not a valid mutable CFDictionary, the behavior is
		undefined. If the dictionary is a fixed-capacity dictionary and
		it is full before this operation, and the key does not exist in
		the dictionary, the behavior is undefined.
	@param key The key of the value to set into the dictionary. If a key 
		which matches this key is already present in the dictionary, only
		the value is changed ("add if absent, replace if present"). If
		no key matches the given key, the key-value pair is added to the
		dictionary. If added, the key is retained by the dictionary,
		using the retain callback provided
		when the dictionary was created. If the key is not of the sort
		expected by the key retain callback, the behavior is undefined.
	@param value The value to add to or replace into the dictionary. The value
		is retained by the dictionary using the retain callback provided
		when the dictionary was created, and the previous value if any is
		released. If the value is not of the sort expected by the
		retain or release callbacks, the behavior is undefined.
*/
        CFDictionarySetValue(classesToMatch,
                             CFSTR(kIOSerialBSDTypeKey),
                             CFSTR(kIOSerialBSDAllTypes));

        // Each serial device object has a property with key
        // kIOSerialBSDTypeKey and a value that is one of kIOSerialBSDAllTypes,
        // kIOSerialBSDModemType, or kIOSerialBSDRS232Type. You can experiment with the
        // matching by changing the last parameter in the above call to CFDictionarySetValue.
        
        // As shipped, this sample is only interested in modems,
        // so add this property to the CFDictionary we're matching on. 
        // This will find devices that advertise themselves as modems,
        // such as built-in and USB modems. However, this match won't find serial modems.
    }
    
    /*! @function IOServiceGetMatchingServices
        @abstract Look up registered IOService objects that match a matching dictionary.
        @discussion This is the preferred method of finding IOService objects currently registered by IOKit. IOServiceAddNotification can also supply this information and install a notification of new IOServices. The matching information used in the matching dictionary may vary depending on the class of service being looked up.
        @param masterPort The master port obtained from IOMasterPort().
        @param matching A CF dictionary containing matching information, of which one reference is consumed by this function. IOKitLib can contruct matching dictionaries for common criteria with helper functions such as IOServiceMatching, IOOpenFirmwarePathMatching.
        @param existing An iterator handle is returned on success, and should be released by the caller when the iteration is finished.
        @result A kern_return_t error code. */

    kernResult = IOServiceGetMatchingServices(masterPort, classesToMatch, matchingServices);    
    if (KERN_SUCCESS != kernResult)
    {
        printf("IOServiceGetMatchingServices returned %d.\n", kernResult);
		goto exit;
    }
        
exit:
    return kernResult;
}


static kern_return_t MatchModemPath(io_iterator_t serialPortIterator,
									const char *requestedPortPath,
									int *port,
									CFIndex maxPathSize)
{
    io_object_t		modemService;
    kern_return_t	kernResult = KERN_FAILURE;
	char *bsdPath;
	
	*port = 1;
	bsdPath = (char*)malloc(sizeof(char) * maxPathSize);
    bsdPath[0] = '\0';
    
    // Iterate across all modems found. In this example, we bail after finding the first modem.
    while (modemService = IOIteratorNext(serialPortIterator)) {
        CFTypeRef	bsdPathAsCFString;

		// Get the callout device's path (/dev/cu.xxxxx). The callout device should almost always be
		// used: the dialin device (/dev/tty.xxxxx) would be used when monitoring a serial port for
		// incoming calls, e.g. a fax listener.
		bsdPathAsCFString = IORegistryEntryCreateCFProperty(modemService,
                                                            CFSTR(kIOCalloutDeviceKey),
                                                            kCFAllocatorDefault,
                                                            0);
															
        // Release the io_service_t now that we are done with it.
		(void) IOObjectRelease(modemService);
		
        if (bsdPathAsCFString) {
            Boolean result;
            
            // Convert the path from a CFString to a C (NUL-terminated) string for use
			// with the POSIX open() call.
			result = CFStringGetCString(bsdPathAsCFString,
										bsdPath,
										maxPathSize, 
										kCFStringEncodingASCII);
			CFRelease(bsdPathAsCFString);
				
			if (result) {
//              mexPrintf("BSD path: %s\n", bsdPath);
// 				mexPrintf("Desired path: %s\n", requestedPortPath);
// 				mexPrintf("port: %d\n", *port);
				
				if (strcmp(bsdPath, requestedPortPath) == 0) {
					kernResult = KERN_SUCCESS;
					break;
				}
				else {
					(*port)++;
				}
            }
        } // End if (bsdPathAsCFString)
    }
	
	free(bsdPath);
        
    return kernResult;
}


    
// Given an iterator across a set of modems, return the BSD path to the first one.
// If no modems are found the path name is set to an empty string.
//
static kern_return_t GetModemPath(io_iterator_t serialPortIterator, 
								  int			port, 
								  char			*bsdPath, 
								  CFIndex		maxPathSize )
{
    io_object_t		modemService;
    kern_return_t	kernResult = KERN_FAILURE;
	
    // Initialize the returned path
	//
    *bsdPath = '\0';
    
    // Iterate across all modems found. In this example, we bail after finding the first modem.
    //
    while( modemService = IOIteratorNext(serialPortIterator) )
    {
        CFTypeRef	bsdPathAsCFString;

		// Get the callout device's path (/dev/cu.xxxxx). The callout device should almost always be
		// used: the dialin device (/dev/tty.xxxxx) would be used when monitoring a serial port for
		// incoming calls, e.g. a fax listener.
	
		bsdPathAsCFString = IORegistryEntryCreateCFProperty(modemService,
                                                            CFSTR(kIOCalloutDeviceKey),
                                                            kCFAllocatorDefault,
                                                            0);
															
        // Release the io_service_t now that we are done with it.
	
		(void) IOObjectRelease(modemService);
		
        if (bsdPathAsCFString)
        {
            Boolean result;
            
            // Convert the path from a CFString to a C (NUL-terminated) string for use
			// with the POSIX open() call.
	    
			result = CFStringGetCString(bsdPathAsCFString,
											bsdPath,
											maxPathSize, 
											kCFStringEncodingASCII);
			CFRelease(bsdPathAsCFString);
				
			if (result)
			{
//                 mexPrintf("BSD path: %s\n", bsdPath);
// 				mexPrintf("port: %d\n", port);
				if( port==0 ) {
					kernResult = KERN_SUCCESS;
					break;
				}
				port--;
            }
        }

    }
        
    return kernResult;
}



// Given the port for a serial device, close that device.
void closeSerialPort(PORTINFO *info)
{
    // Block until all written output has been sent from the device.
    // Note that this call is simply passed on to the serial device driver.
    // See tcsendbreak(3) ("man 3 tcsendbreak") for details.
    if (tcdrain( info->fd ) == -1)
    {
        printf("Error waiting for drain - %s(%d).\n",
            strerror(errno), errno);
    }
    
    // Traditionally it is good practice to reset a serial port back to
    // the state in which you found it. This is why the original termios struct
    // was saved.
    if (tcsetattr( info->fd, TCSANOW, &info->originalTTYAttrs) == -1)
    {
        printf("Error resetting tty attributes - %s(%d).\n",
            strerror(errno), errno);
    }
    close( info->fd );
}


// Converts a port string name into a port integer.
int portName2Number(const char *portName, PORTINFO *info)
{
	kern_return_t	kernResult;		// on PowerPC this is an int (4 bytes)
	io_iterator_t	serialPortIterator;
	int port;
	
	kernResult = FindModems(&serialPortIterator);
	kernResult = MatchModemPath(serialPortIterator, portName, &port, sizeof(info->bsdPath));
	
	IOObjectRelease(serialPortIterator);	// Release the iterator.
	
	if (kernResult == KERN_FAILURE) {
		//mexPrintf("No modem found.\n");
		goto error;
	}
	
	return port;
	
	// Failure path
error:
	if (info->fd != -1) {
		close(info->fd);
		info->fd = -1;
	}
	return -1;
}


int openSerialPort(int port, PORTINFO *info, char *config)
{
    kern_return_t	kernResult;		// on PowerPC this is an int (4 bytes)
    io_iterator_t	serialPortIterator;
    int				handshake;
	struct termios  options;
	
	
    kernResult = FindModems(&serialPortIterator);
    
    kernResult = GetModemPath(serialPortIterator, port, info->bsdPath, sizeof(info->bsdPath));
    
    IOObjectRelease(serialPortIterator);	// Release the iterator.
    
    if (kernResult == KERN_FAILURE) {
        mexPrintf("No modem port found.\n");
		goto error;
    }
	
	//mexPrintf("Opening BSD path: %s\n", info->bsdPath);

    // Now open the modem port we found, initialize the modem, then close it
    info->fd = open( info->bsdPath, O_RDWR | O_NOCTTY | O_NONBLOCK);
    if (info->fd == -1) {
        mexPrintf("Error opening serial port - %s(%d).\n",
			strerror(errno), errno);
        goto error;
    }

    // Note that open() follows POSIX semantics: multiple open() calls to the same file will succeed
    // unless the TIOCEXCL ioctl is issued. This will prevent additional opens except by root-owned
    // processes.
    // See tty(4) ("man 4 tty") and ioctl(2) ("man 2 ioctl") for details.
    if (ioctl(info->fd, TIOCEXCL) == -1) {
        mexPrintf("Error setting TIOCEXCL - %s(%d).\n",
            strerror(errno), errno);
        goto error;
    }

    // Get the current options and save them so we can restore the default settings later.
    if (tcgetattr( info->fd, &info->originalTTYAttrs) == -1) {
        mexPrintf("Error getting tty attributes - %s(%d).\n",
            strerror(errno), errno);
        goto error;
    }

    // The serial port attributes such as timeouts and baud rate are set by modifying the termios
    // structure and then calling tcsetattr() to cause the changes to take effect. Note that the
    // changes will not become effective without the tcsetattr() call.
    // See tcsetattr(4) ("man 4 tcsetattr") for details.
    //
	options = info->originalTTYAttrs;
	
	if( decodeConfigStr( &options, config ) != 0 ) {
        goto error;
	}

    // Cause the new options to take effect immediately.
    if (tcsetattr(info->fd, TCSANOW, &options) == -1)
    {
        mexPrintf("Error setting tty attributes - %s(%d).\n",
            strerror(errno), errno);
        goto error;
    }

    handshake = TIOCM_DTR | TIOCM_RTS | TIOCM_CTS | TIOCM_DSR;
    if (ioctl(info->fd, TIOCMSET, &handshake) == -1)
    // Set the modem lines depending on the bits set in handshake
    {
        mexPrintf("Error setting handshake lines - %s(%d).\n",
            strerror(errno), errno);
    }
    
    return 0;

    
    // Failure path
error:
    if (info->fd != -1)
    {
        close(info->fd);
		info->fd = -1;
    }
    return -1;
}
