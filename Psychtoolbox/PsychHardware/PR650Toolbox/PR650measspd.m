function [spd, qual] = PR650measspd(S)
% [spd,qual] = PR650measspd(S)
%
% Make a measurement of the spectrum.  Tries to be smart
% about using sync or not.
% 
% 8/26/10  dhb  Add DEBUG option to figure out why this dies sometimes.

DEBUG = 1;

global g_serialPort;

% Check for initialization
if isempty(g_serialPort)
   error('Meter has not been initialized.');
end

% Set wavelength sampling if passed.
if nargin < 1 || isempty(S)
   S = [380 5 81];
end

% Initialize
timeout = 30;

% See if we can sync to the source and set sync mode appropriately.
syncFreq = PR650getsyncfreq;
if ~isempty(syncFreq)
	PR650setsyncfreq(1);
else
	PR650setsyncfreq(0);
end

% Do raw read
readStr = PR650rawspd(timeout);

% fprintf('Got data\n');
qual = sscanf(readStr,'%f',1);
	 
% Check for sync mode error condition.  If get one,
% turn off sync and try again.
if qual == 7 || qual == 8
    if (DEBUG)
        fprintf('Got qual code of %d, setting sync freq to 0 and trying again.\n',qual);
    end
	PR650setsyncfreq(0);
	readStr = PR650rawspd(timeout);
	qual = sscanf(readStr, '%f', 1);
    if (DEBUG)
        fprintf('Retry returns quality code of %d\n',qual);
    end
end
	
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
