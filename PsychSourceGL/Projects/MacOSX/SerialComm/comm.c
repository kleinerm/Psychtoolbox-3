/*
	comm.c -- source for Matlab MEX file. Matlab interface to the serial ports
					Tom L. Davis
					created 3-27-04
 MATLAB calls:
 
			 comm( 'open', port [, config] )	% configuration defaults to "19200,n,8,1"
			 comm( 'status', port );			% print port status
			 comm( 'hshake', port, type );		% set serial handshake
			 comm( 'purge',  port );			% purge read and write buffers
			 comm( 'break', port );				% send a break
	array  = comm( 'read', port [, cnt ] );		% read upto CNT unsigned bytes
	string = comm( 'readl', port [,eol] );		% read one ASCII line
			 comm( 'write', port, string );		% write ASCII string
			 comm( 'close', port );
 
	$Id: comm.c,v 1.1.1.1 2004/05/18 15:02:25 tom Exp $
 */


#include "serialPort.h"
#include "mex.h"
#include <stdlib.h>
#include <ctype.h>

#define STRLEN 8192			// size of the read and write buffers (increase if necessary).

// Matlab versions 7.2 and lower don't have the mwSize type so we define
// it as an int for them.
#if MATLAB_VER < 73
//typedef int mwSize;
#endif

// selector numbers

enum {
	OPEN_=1,		// open & close must be first two entries
	CLOSE_,
	READ_,
	READL_,
	WRITE_,
	PURGE_,
	BREAK_,
	HSHAKE_,
	STATUS_,
	TRANS_
};

typedef struct {
	char	name[12];					// selector name
	int		index;						// selector number
} func_info;




// Default port info (add more elements if you need more ports)
//
static PORTINFO portInfo[] = {
{ -1, NULL, 0, 0xA, "" },
{ -1, NULL, 0, 0xA, "" },
{ -1, NULL, 0, 0xA, "" },
{ -1, NULL, 0, 0xA, "" },
{ -1, NULL, 0, 0xA, "" },
{ -1, NULL, 0, 0xA, "" },
{ -1, NULL, 0, 0xA, "" },
{ -1, NULL, 0, 0xA, "" }
};


static func_info func[] = {
{ "read",	 	READ_	},
{ "readl",	 	READL_	},
{ "write", 		WRITE_	},
{ "status",	 	STATUS_ },
{ "purge", 		PURGE_	},
{ "break", 		BREAK_	},
{ "open",	 	OPEN_   },
{ "close", 		CLOSE_	},
{ "hshake", 	HSHAKE_	},
{ "name2number", TRANS_ },
{ "", 			NULL	}
};

static char		*string = NULL;



// Close all ports if SERIALCOMM gets cleared or MATLAB exits
//
static void die( void )
{
	int		port;
	
	mexPrintf( "SERIALCOMM 'die'\n" );
	if( string != NULL ) {
		free(string);
		string = NULL;
	}
	for( port=0; port<sizeof(portInfo)/sizeof(PORTINFO); port++ ) {
		if( portInfo[port].fd != -1 ) {
			closeSerialPort( &portInfo[port] );
			portInfo[port].fd = -1;
			free( portInfo[port].readBufferPtr );
			portInfo[port].readBufferPtr = NULL;
		}
	}
}




void mexFunction(int nlhs, mxArray *plhs[], int nrhs, const mxArray	*prhs[])
{
	ssize_t		numBytes;		// Number of bytes read or written
	long		i, cnt, str_cnt;
	int			n, port, t_port;
	PORTINFO	*p;
	double		*x;
	mwSize		dims[2] = { 0, 1 };
	char		*str;
	char		op[16], comArg[256];
	
	
	// Check for at least two input arguments
	if (nrhs < 2) {
		mexErrMsgTxt("SERIALCOMM: requires at least two input arguments.");
	}
	
	// Read selector
	if(mxGetString(prhs[0], op, sizeof(op))) {
		mexErrMsgTxt("SERIALCOMM: bad first argument.");
	}
	
	// If the second argument is a number, then it's a port number.  Otherwise,
	// it should be a string.
	if (mxIsDouble(prhs[1])) {
		port = (int)mxGetScalar(prhs[1]) - 1;
		if ((port < 0) | (port > sizeof(portInfo)/sizeof(PORTINFO))) {
			mexErrMsgTxt( "SERIALCOMM: port number out of range." );
		}
		p = &portInfo[port];
	}
	else if (mxIsChar(prhs[1])) {
		if (mxGetString(prhs[1], comArg, 255)) {
			mexErrMsgTxt("SERIALCOMM: failed to allocate string memory.");
		}
	}
	else {
		mexErrMsgTxt("SERIALCOMM: invalid second argument type.");
	}
	
	
	// Find selector
	
	for( i=0; i<sizeof(func)/sizeof(func_info)-1; i++ ) {
		if( strcmp( op, func[i].name ) == 0 ) {
			break;
		}
	}
	
	if( (p->fd == -1) & (func[i].index > CLOSE_) ) {
		mexErrMsgTxt("SERIALCOMM: device not open.");
	}
	
	switch( func[i].index ) {
		
		
		// ******************************* OPEN *********************************
		//
		//	comm( 'open', port )			% port configuration defaults to "19200,n,8,1"
		//	comm( 'open', port, config )	% use CONFIG value for port configuration (see
		//									% 'comm.m' for legal values of CONFIG.)
		
		case OPEN_:
			
			if( nrhs < 2 ){
				mexErrMsgTxt("SERIALCOMM('OPEN'): requires at two input arguments.");
			}
			
			
			if( p->fd != -1 ) {
				mexPrintf( "SERIALCOMM('OPEN'): port %d already open.\n", port+1 );
				return;
			}
			
			// setup the string buffer
			//
			if (string == NULL) {
				string = calloc(STRLEN, sizeof(char));
				if (string == 0) {
					mexErrMsgTxt("SERIALCOMM('OPEN'): string buffer allocation failed.");
				}
			}
			
			// setup the read buffer
			//
			p->readBufferPtr = calloc(STRLEN, sizeof(char));
			if (p->readBufferPtr == 0) {
				mexErrMsgTxt("SERIALCOMM('OPEN'): working buffer allocation failed.");
			}
			
			
			// configure the port
			//
			{
				char	configStr[16] = "19200,n,8,1";
				if (nrhs == 3) {
					if (mxGetString( prhs[2], configStr, sizeof(configStr))) {
						mexErrMsgTxt("SERIALCOMM('OPEN'): bad third argument.");
					}
				}
				
				if (openSerialPort(port, p, configStr)) {
					mexErrMsgTxt("SERIALCOMM('OPEN'): could not open port.");
				}
			}
			
			// register an exit routine
			
			if( mexAtExit( die ) != 0 ) {
				die();
				mexErrMsgTxt("SERIALCOMM('OPEN'): failed to register exit routine.");
			}
			
			p->str_cnt = 0;
			mexPrintf( "Opened device: '%s'\n", p->bsdPath );
			return;
			
			
			// ******************************* STATUS **********************************
			//
			//	comm( 'status', port );			% print port status
			
		case STATUS_:
			
			if (nrhs != 2) {
				mexErrMsgTxt("SERIALCOMM('STATUS'): requires two input arguments.");
			}
			
			
			if( portStatus( p ) ) {
				mexErrMsgTxt("SERIALCOMM('STATUS'):failed.");
			}
			
			return;
			
		case TRANS_:
			
			// ******************************* TRANSLATE **********************************
			//
			//	comm( 'name2number', portName );	% Converts a port name to a port number.
			if (nrhs != 2) {
				mexErrMsgTxt("SERIALCOMM('NAME2NUMBER'): requires two input arguments.");
			}
			
			t_port = portName2Number((const char*)comArg, p);
			if (t_port == -1) {
				mexErrMsgTxt("SERIALCOMM('NAME2NUMBER'): could not convert name to a port number.");
			}
			
			plhs[0] = mxCreateDoubleScalar((double)t_port);
			
			
			return;
			
			// ******************************* BREAK **********************************
			//
			//	comm( 'break', port );				% send a break
			
		case BREAK_:
			
			if( nrhs != 2 ){
				mexErrMsgTxt("SERIALCOMM('BREAK'): requires two input arguments.");
			}
			
			if( sendBreak( p ) ) {
				mexErrMsgTxt("SERIALCOMM('BREAK'):failed.");
			}
			
			// flush read/write queues
			//
			if( flushPort( p ) ) {
				mexErrMsgTxt("SERIALCOMM('BREAK'): port purge failed.");
			}
			p->str_cnt = 0;
			
			return;
			
			
			// ******************************* HSHAKE **********************************
			//
			//	comm( 'hshake', port, type );		% set serial handshake
			
		case HSHAKE_:
			
			if( nrhs != 3 ){
				mexErrMsgTxt("SERIALCOMM('HSHAKE'): requires three input arguments.");
			}
			
			if( mxGetString( prhs[2], string, STRLEN ) ) {
				mexErrMsgTxt("SERIALCOMM('HSHAKE'): bad third argument.");
			}
			
			
			// Setup handshaking
			//
			if (setHandshake(p, string)) {
				mexErrMsgTxt("SERIALCOMM('HSHAKE'): failed.");
			}
			
			
			// flush read/write queues
			//
			if( flushPort( p ) ) {
				mexErrMsgTxt("SERIALCOMM('HSHAKE'): port purge failed.");
			}
			p->str_cnt = 0;
			
			return;
			
			
			// ******************************* PURGE **********************************
			//
			//	comm( 'purge', port );			% purge read and write buffers
			
		case PURGE_:
			
			if( nrhs != 2 ){
				mexErrMsgTxt("SERIALCOMM('PURGE'): requires two input arguments.");
			}
			
			// flush read/write queues
			//
			if( flushPort( p ) ) {
				mexErrMsgTxt("SERIALCOMM('PURGE'): port purge failed.");
			}
			p->str_cnt = 0;
			
			return;
			
			
			// ******************************* READ **********************************
			//
			//	data = comm( 'read', port );		% Binary read of everything in buffer
			//	data = comm( 'read', port, cnt );	% Binary read of upto CNT bytes
			
		case READ_:
			
			if( nrhs < 2 ) {
				mexErrMsgTxt("SERIALCOMM('READ'): requires at least two input arguments.");
			}
			
			
			// find out how many bytes we should try to read
			//
			if( nrhs == 3 ) {
				cnt = (long) mxGetScalar( prhs[2] );
				cnt = MIN( cnt, STRLEN-p->str_cnt );
			} else {
				cnt = STRLEN-p->str_cnt;
			}
			
			
			// read the port into the read buffer
			//
			numBytes = read(p->fd, &p->readBufferPtr[p->str_cnt], cnt);
			if( numBytes == -1 )
			{
				mexPrintf("Error reading serial port - %s(%d).\n", strerror(errno), errno);
				mexErrMsgTxt("SERIALCOMM('READ'): device read failed.");
			}
			p->str_cnt += numBytes;
			
			
			// Create a matrix for the return data
			//
			dims[0] = p->str_cnt;
			plhs[0] = mxCreateNumericArray( 2, dims, mxUINT8_CLASS, mxREAL );
			if( plhs[0]==0 ) {
				mexErrMsgTxt("SERIALCOMM('READ'): mxCreateNumericArray failed.");
			}
			
			
			// move the data into the matrix & clear the read buffer
			//
			memmove(mxGetData(plhs[0]), p->readBufferPtr, p->str_cnt);
			p->str_cnt = 0;
			
			return;
			
			
			// ******************************* READ LINE **********************************
			//
			// string = comm( 'readl', port );			% read one line of ASCII data
			// string = comm( 'readl', port, eol );		% read one line of ASCII data
			
		case READL_:
			
			if( nrhs < 2 ) {
				mexErrMsgTxt("SERIALCOMM('READL'): requires at least two input arguments.");
			}
			
			if( nrhs == 3 ) {
				p->eol = mxGetScalar( prhs[2] );
			}
			
			str_cnt = p->str_cnt;
			
			numBytes = read( p->fd, &p->readBufferPtr[str_cnt], STRLEN-str_cnt );
			if( numBytes == -1 )
			{
				mexPrintf("Error reading serial port - %s(%d).\n", strerror(errno), errno);
				mexErrMsgTxt("SERIALCOMM('READ'): device read failed.");
			}
			
			str_cnt += numBytes;					// cur # bytes in 'readBufferPtr'
			
			for( i=0; i<str_cnt; i++ ) {
				if( p->readBufferPtr[i] == p->eol ) {
					p->readBufferPtr[i] = 0;
					plhs[0] = mxCreateString( p->readBufferPtr );
					if( plhs[0]==0 ) {
						mexErrMsgTxt("SERIALCOMM('READL'): mxCreateString failed.");
					}
					n = mxGetN(plhs[0]) * mxGetM(plhs[0]);
					if( i != n ) {
						mexPrintf( "SERIALCOMM('READL'): found null instead of EOL\n" );
						p->readBufferPtr[i] = p->eol;
					}
					str_cnt -= n + 1;
					memmove( p->readBufferPtr, &p->readBufferPtr[n+1], str_cnt );
					p->str_cnt = str_cnt;
					return;
				}
			}
			
			// is the buffer full with no EOL?
			
			if( str_cnt==STRLEN ) {
				str_cnt = 0;							// clear buffer
				mexErrMsgTxt("SERIALCOMM('READL'): no EOL found, buffer overflow.");
			}
			
			// Create a matrix for the return data
			
			plhs[0] = mxCreateString( "" );
			if( plhs[0]==0 ) {
				mexErrMsgTxt("SERIALCOMM('READL'):' mxCreateString failed.");
			}
			
			p->str_cnt = str_cnt;
			return;
			
			
			// ******************************* WRITE ********************************
			//
			//	comm( 'write', port, string );		% write string to PORT
			
		case WRITE_:
			
			if( nrhs != 3 ) {
				mexErrMsgTxt("SERIALCOMM('WRITE'): requires three input arguments.");
			}
			
			// Read input parameters
			
			cnt = mxGetN( prhs[2] ) * mxGetM( prhs[2] );
			if( cnt > STRLEN ) {
				mexErrMsgTxt("SERIALCOMM('WRITE'): third argument is too large.");
			}
			
			if( mxIsChar( prhs[2] ) ) {				// copy Matlab character data into string
				
				if( mxGetString( prhs[2], string, STRLEN ) ) {
					mexErrMsgTxt("SERIALCOMM('WRITE'): bad third argument.");
				}
				str = string;
				
				
			} else if( mxIsInt8( prhs[2] ) ) {		//
				str = mxGetData(prhs[2]);
				
				
			} else if( mxIsDouble( prhs[2] ) ) {	// copy Matlab matrix data into string
				x = mxGetPr( prhs[2] );
				for( i=0; i<cnt; i++ ) {
					string[i] = (char) *x++;
				}
				str = string;
				
				
			} else {
				mexErrMsgTxt("SERIALCOMM('WRITE'): bad third argument.");
				
			}
			
			// write 'string' to serial device
			
			numBytes = write( p->fd, str, cnt );
			if( numBytes == -1) {
				mexPrintf("Error writing to modem - %s(%d).\n", strerror(errno), errno);
				mexErrMsgTxt("SERIALCOMM('WRITE'): device write failed.");
			}
			
			
			// return number of bytes written to port
			//
			plhs[0] = mxCreateDoubleMatrix( 1, 1, mxREAL );
			if( plhs[0]==0 ) {
				mexErrMsgTxt("SERIALCOMM('WRITE'): mxCreateDoubleMatrix failed.");
			}
			x = mxGetPr( plhs[0] );
			*x = numBytes;
			
			return;
			
			
			// ******************************* CLOSE ********************************
			//
			//	comm( 'close', port );
			
		case CLOSE_:
			if( p->fd == -1 ) {
				mexPrintf("SERIALCOMM('CLOSE'): port %d is not open.\n", port+1 );
				return;
			}
			
			closeSerialPort( p );
			p->fd = -1;
			p->str_cnt = 0;
			free( p->readBufferPtr );
			p->readBufferPtr = NULL;
			return;
			
			
			default:
				mexErrMsgTxt("SERIALCOMM: bad first argument.");
				
	}
}
