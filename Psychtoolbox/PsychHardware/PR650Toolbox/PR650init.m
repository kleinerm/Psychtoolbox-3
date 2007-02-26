function retval = PR650init(portNumber, enableHandshaking)
% retval = PR650init(portNumber, [enableHandshaking])
% 
% Initialize serial port for talking to colorimeter.
% Returns whatever character is sent by colorimeter
%
% 'enableHandshaking' allows you to enable handshaking.  By default,
% handshaking is disabled.  To enable handshaking, set this value to 1 or
% true.
 
global g_serialPort;

if nargin == 1
    enableHandshaking = 0;
end

if enableHandshaking
    handshakeCode = 'h';
else
    handshakeCode = 'n';
end

% Only open if we haven't already.
if isempty(g_serialPort)
   SerialComm('open', portNumber, '9600,n,8,1');
   SerialComm('hshake', portNumber, handshakeCode);
   SerialComm('close', portNumber);
   WaitSecs(0.5);
   SerialComm('open', portNumber, '9600,n,8,1');
   SerialComm('hshake', portNumber, handshakeCode);
   g_serialPort = portNumber;
end

% Send set backlight command to high level to check
% whether we are talking to the meter.
SerialComm('write', portNumber, ['b3' char(10)]);
retval = [];
while isempty(retval)
    retval = PR650serialread;
end
