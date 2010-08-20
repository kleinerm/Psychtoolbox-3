function syncFreq = PR650getsyncfreq
% syncFreq = PR650getsyncfreq
%
% Measure sync frequency for source.  Returns
% empty if can't sync.
%
% 8/20/10  Change error message to match command sent by this routine.

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


% Make measurement
% fprintf('Measure\n');
if g_useIOPort
	IOPort('write', g_serialPort, ['f' char(10)]);
else
	SerialComm('write', g_serialPort, ['f' char(10)]);
end

waited = 0;
inStr = [];
while isempty(inStr) && (waited < timeout)
	WaitSecs(1);
	waited = waited + 1;
	inStr = PR650serialread;
end
if waited == timeout
	error('No response after f command');
end

% Pick up entire buffer.  This is the loop referred to above.
readStr = inStr;
while ~isempty(inStr)
	inStr = PR650serialread;
	readStr = [readStr inStr];
end

% Parse return
qual = -1;
[raw, count] = sscanf(readStr,'%f,%f',2);
if count == 2
	qual = raw(1);
	syncFreq = raw(2);
end

if qual ~= 0
	syncFreq = [];
end
