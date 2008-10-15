function y = SerialComm(op, port, data)

% SerialComm -- Serial port interface for OSX
%  SerialComm( 'open', PORT, CONFIG ) opens comm port number PORT for reading and
%    writing. The CONFIG string specifies the basic serial port (baud rate, 
%    parity, #data bits, #stop bits) in standard DOS format. CONFIG defaults 
%    to '19200,n,8,1'.
%
%  STR = SerialComm( 'readl', PORT, EOL ) reads one line of ASCII text from PORT
%    and returns the line in the string array STR. If a complete line is 
%    not available, STR is empty. If supplied, EOL defines the End-of-Line 
%    character which remains in effect until changed. On open, the EOL 
%    character is the ASCII line-feed (0xA). Non-blocking.
%   
%  DATA = SerialComm( 'read', PORT, N ) reads upto N bytes from PORT and returns 
%    the uint8 array in DATA. If no data is available, DATA is empty. If N is
%    not specified, all available bytes are returned. Non-blocking.
%   
%  SerialComm( 'write', PORT, DATA ) writes contents of the matrix DATA to PORT. 
%    The matrix DATA can be of class "double" or "char".
%   
%  SerialComm( 'purge', PORT ) purges read and write buffers for the PORT.
%	
%  SerialComm( 'hshake', PORT, HSHAKE ) set hardware 'h' and/or software 's' hand-
%    shaking. 'n' sets handshaking to none.
%	
%  SerialComm( 'break', PORT ) sends a break.
%	
%  SerialComm( 'close', PORT ) closes the PORT. 
%
%  SerialComm( 'status', PORT ) prints some status info. 



