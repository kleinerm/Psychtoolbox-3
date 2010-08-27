function PR650setsyncfreq(syncFreq)
% PR650setsyncfreq(syncFreq)
%
% Set sync frequency for source.
%   Passing 0 means don't use sync mode.
%   Passing 1 means use last sync measurement.
%
% See also PR650getsyncfreq
%
% 8/20/10  Change error message to match command sent by this routine.
% 8/26/10  Better usage comment at top.

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

% Set
if g_useIOPort
	if (syncFreq ~= 0)
		IOPort('write', g_serialPort, ['s01,,,,' num2str(syncFreq) ',0,01,1' char(10)]);
	else
		IOPort('write', g_serialPort, ['s01,,,,' ',0,01,1' char(10)]);
	end
else
	if (syncFreq ~= 0)
		SerialComm('write', g_serialPort, ['s01,,,,' num2str(syncFreq) ',0,01,1' char(10)]);
	else
		SerialComm('write', g_serialPort, ['s01,,,,' ',0,01,1' char(10)]);
	end
end

waited = 0;
inStr =[];
while isempty(inStr) && (waited < timeout)
	WaitSecs(1);
	waited = waited+1;
	inStr = PR650serialread;
end
if waited == timeout
	error('No response after s01 command');
end

% Pick up entire buffer.  This is the loop referred to above.
readStr = inStr;
while ~isempty(inStr)
	inStr = PR650serialread;
	readStr = [readStr inStr]; %#ok<AGROW>
end

% Parse return
qual = sscanf(readStr, '%f', 1);
if qual ~= 0
	fprintf('Return string was %s\n', readStr);
	error('Can''t set parameters');
end
