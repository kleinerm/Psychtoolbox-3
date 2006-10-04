function [xyz,qual] = PR650measxyz
% [xyz,qual] = PR650measspd
%
% Make a measurement of the spectrum.

global g_serialPort;

% Check for initialization
if isempty(g_serialPort)
   error('Meter has not been initialized.');
end

% Initialize
timeout = 30;

% See if we can sync to the source
% and set sync mode appropriately.
syncFreq = PR650getsyncfreq;
if ~isempty(syncFreq)
	PR650setsyncfreq(1);
else
	PR650setsyncfreq(0);
end

% Make measurement and get the string;
readStr = PR650rawxyz(timeout);
	
% fprintf('Got data\n');
qual = sscanf(readStr, '%f', 1);
		 
% Check for sync mode error condition.  If get one,
% turn off sync and measure again.
if qual == 7 || qual == 8
	PR650setsyncfreq(0);
	readStr = PR650rawxyz(timeout);
	qual = sscanf(readStr, '%f', 1);
end
	
% Check for other error conditions
if qual == -1 || qual == 10
  %disp('Low light level during measurement');
  %disp('Setting returned value to zero');
  xyz = zeros(3,1);
elseif qual == 18 || qual == 0
	units = str2num(readStr(4));
	if units ~= 0
		error('Units not returned as cd/m2');
	end
	X = str2num(readStr(6:14));
	Y = str2num(readStr(16:24));
	Z = str2num(readStr(26:34));
	xyz = [X ; Y ; Z];
elseif (qual ~= 0)
  error('Bad return code %g from meter',qual);
end	

