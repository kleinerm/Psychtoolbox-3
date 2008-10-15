// $Id: serialPort.h,v 1.1.1.1 2004/05/18 15:02:25 tom Exp $

#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <sys/param.h>
#include <termios.h>


typedef struct {
	int				fd;						// serial port file descriptor
	char			*readBufferPtr;			// read buffer
	int				str_cnt;				// number of bytes in working buffer
	char			eol;					// EOL character for READL
	char			bsdPath[MAXPATHLEN];	// BSD device name
	struct termios  originalTTYAttrs;		// original termios attributes
} PORTINFO;



int openSerialPort( int port, PORTINFO *info, char *config );
int setHandshake( PORTINFO *info, char *string );
int flushPort( PORTINFO *info );
int sendBreak( PORTINFO *info );
int portStatus( PORTINFO *info );
void closeSerialPort( PORTINFO *info );
