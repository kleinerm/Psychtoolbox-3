function retval = PR655init(portNumber)
% retval = PR655init(portNumber, [enableHandshaking])
% 
% Initialize serial port for talking to colorimeter.
% Returns whatever character is sent by colorimeter
%
% Per PhotoResearch, handshaking is disabled.
%
% 11/26/07    mpr   added timeout if nothing is returned within 10 seconds.
% 01/16/09    tbc   Adapted from PR650Toolbox for use with PR655
%
% It seems the iterative CMCheckInit method of initialization is not
% necessary with the PR655, so one run of this seems to do the trick.
% "usbmodem*" seems to cover every instance I've run into. Not sure about
% the prevalance of using usbmodem as a generic identifier, but if you can
% afford the PR655, you're probably on some more respectable form of internet
% connection. -TBC
% 
 
global g_serialPort;

% If useIOPort == 1 the IOPort driver shall be used instead of SerialComm:
useIOPort = 0;

if nargin == 0
    % This seems to be the default name on OS/X. We don't know about other
    % operating systems defaults:
    portNumber = FindSerialPort('usbmodem');
end

% Only open if we haven't already.
if isempty(g_serialPort)
    if ~useIOPort
        SerialComm('open', portNumber, '9600,n,8,1');
        SerialComm('hshake', portNumber, 'n');
        g_serialPort = portNumber;
    else
        % IOPort has above port settings 9600 baud, no parity, 8 data bits,
        % 1 stopbit, no handshake (aka FlowControl=none) already as
        % built-in defaults, so no need to pass them:
        oldverbo = IOPort('Verbosity', 2);
        g_serialPort = IOPort('OpenSerialPort', portNumber);
        IOPort('Verbosity', oldverbo);        
    end
end

% Put in Remote Mode --No [CR] after 'PHOTO'
rm = 'PHOTO';
for i = 1:5
    if ~useIOPort
        SerialComm('write', g_serialPort, rm(i));
    else
        IOPort('write', g_serialPort, rm(i));
    end
end

if ~useIOPort
    StartTime = GetSecs;
    retval = [];
    while isempty(retval) && GetSecs-StartTime < 10
        retval = PR655read;
    end
else
    % Ok, fake this for IOPort mode. Just return something that keeps us
    % going.
    retval ='0';
    
    % Of course all successive code will fail! The proper approach if
    % everything up to this point works is to simply replace all calls to
    % SerialComms 'Write', 'Read' and 'Close' functions by IOPort. I.e.,
    % just find all calls and replace the function name SerialComm by the
    % function name IOPort. Syntax and behaviour of IOPort should be
    % exactly the same, so a simple text search & replace should do the
    % job...
end

return;
