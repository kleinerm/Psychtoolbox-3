function readStr = PR650rawxyz(timeout)
% readStr = PR650rawxyz(timeout)
%
% Make a raw XYZ measurement.

global g_serialPort;

% Check for initialization
if isempty(g_serialPort)
   error('Meter has not been initialized.');
end

% Flushing buffers.
% fprintf('Flush\n');
dumpStr = '0';
while ~isempty(dumpStr)
	dumpStr = PR650serialread;
end

% Make measurement
% fprintf('Measure\n');
SerialComm('write', g_serialPort, ['m0' char(10)]);
waited = 0;
inStr = [];
while isempty(inStr) && (waited < timeout)
	WaitSecs(1);
	waited = waited+1;
	inStr = PR650serialread;
end
if waited == timeout
	error('No response after measure command');
end

% Get the data.  In this first loop, we make
% sure something came back from the meter.  If
% something did come back, we need to loop (below)
% to pick up the entire buffer, because some serial
% ports seem to be set up so that PsychSerial('Read',...)
% only reads to the EOL char.  May be able to change
% this by tweaking PsychSerial, but for now we handle it here.
% fprintf('Get data\n');
SerialComm('write', g_serialPort, ['d2' char(10)]);
WaitSecs(0.1);
waited = 0;
inStr = [];
while isempty(inStr) && (waited < timeout)
    inStr = PR650serialread;
    WaitSecs(1);
    waited = waited+1;
end
 
if waited == timeout
   error('Unable to get reading from radiometer');
else
	% Pick up entire buffer.  This is the loop referred to above.
	readStr = inStr;
	while ~isempty(inStr)
		inStr = PR650serialread;
		readStr = [readStr inStr];
	end
end
