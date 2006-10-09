function [spd,qual,readStr] = PR650retrievespd(filenumber,S)
% [spd,qual,readStr] = PR650rawspd(filenumber,[S])
%
% Retrieve spd stored in given filenumber.

global g_serialPort;

% Set wavelength sampling if passed.
if nargin < 2 || isempty(S)
   S = [380 5 81];
end

% Set timeout
timeout = 30;

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


% Get the data.  In this first loop, we make
% sure something came back from the meter.  If
% something did come back, we need to loop (below)
% to pick up the entire buffer, because some serial
% ports seem to be set up so that PsychSerial('Read',...)
% only reads to the EOL char.  May be able to change
% this by tweaking PsychSerial, but for now we handle it here.
% fprintf('Get data\n');
SerialComm('write', g_serialPort, ['r' num2str(filenumber) ',5' char(10)]);
WaitSecs(0.1);
waited = 0;
inStr = [];
while isempty(inStr) && (waited < timeout)
    inStr = PR650serialread;
    WaitSecs(1);
    waited = waited+1;
end
 
if (waited==timeout)
   error('Unable to get reading from radiometer');
else
	% Pick up entire buffer.  This is the loop referred to above.
	readStr = inStr;
	while ~isempty(inStr)
		inStr = PR650serialread;
		readStr = [readStr inStr];
	end
end

% fprintf('Got data\n');
qual = sscanf(readStr, '%f', 1);
	 
% Check for other error conditions
if qual == -1 || qual == 10
  %disp('Low light level during measurement');
  %disp('Setting returned value to zero');
  spd = zeros(S(3), 1);
elseif qual == 18 || qual == 0
	spd = PR650parsespdstr(readStr, S);
	
elseif qual ~= 0
  error('Bad return code %g from meter', qual);
end	
