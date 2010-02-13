function serialnumStr = PR650getserialnumber
% serialnumStr = PR650getserialnumber
%
% Get the serial number from the PR-650.  Sometimes
% it's useful to be able to find out 
% which of the instruments in the lab was
% used for any particular measurement.
%
% 2/13/10  dhb  Wrote it, based on PR650getsyncfrequency.

global g_serialPort g_useIOPort;

% Check for initialization
if isempty(g_serialPort)
   error('Meter has not been initialized.');
end

% Initialize
timeout = 30;

% Flushing buffers.
% fprintf('Flush\n');
dumpStr = '0';
while ~isempty(dumpStr)
	dumpStr = PR650serialread;
end

% Send command
if g_useIOPort
	IOPort('write', g_serialPort, ['d110' char(10)]);
else
	SerialComm('write', g_serialPort, ['d110' char(10)]);
end

% Get at least one character
waited = 0;
inStr = [];
while isempty(inStr) && (waited < timeout)
	WaitSecs(1);
	waited = waited + 1;
	inStr = PR650serialread;
end
if waited == timeout
	error('No response from meter');
end

% Pick up entire buffer.
readStr = inStr;
while ~isempty(inStr)
	inStr = PR650serialread;
	readStr = [readStr inStr];
end

% Parse return.  This may contain
% training blanks.
serialnumStr = readStr(1:8);
