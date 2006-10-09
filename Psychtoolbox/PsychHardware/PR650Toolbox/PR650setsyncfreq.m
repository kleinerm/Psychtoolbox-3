function PR650setsyncfreq(syncFreq)
% PR650setsyncfreq(syncFreq)
%
% Set sync frequency for source.  Passing
% 1 means use last sync measurement.
%
% See also PR650getsyncfreq

global g_serialPort;

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

% Set
if (syncFreq ~= 0)
	SerialComm('write', g_serialPort, ['s01,,,,' num2str(syncFreq) ',0,01,1' char(10)]);
else
	SerialComm('write', g_serialPort, ['s01,,,,' ',0,01,1' char(10)]);
end

waited = 0;
inStr =[];
while isempty(inStr) && (waited < timeout)
	WaitSecs(1);
	waited = waited+1;
	inStr = PR650serialread;
end
if waited == timeout
	error('No response after measure command');
end

% Pick up entire buffer.  This is the loop referred to above.
readStr = inStr;
while ~isempty(inStr)
	inStr = PR650serialread;
	readStr = [readStr inStr];
end

% Parse return
qual = sscanf(readStr, '%f', 1);
if qual ~= 0
	fprintf('Return string was %s\n', readStr);
	error('Can''t set parameters');
end
