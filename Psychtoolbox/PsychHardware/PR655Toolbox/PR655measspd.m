function [spd, qual] = PR655measspd(S)
% [spd,qual] = PR655measspd(S)
%
% Make a measurement of the spectrum.
% 
% 01/16/09    tbc   Adapted from PR650Toolbox for use with PR655
%

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

% See if we can sync to the source
% and set sync mode appropriately.
syncFreq = PR655getsyncfreq;
if ~isempty(syncFreq)
	PR655write('SS1');
else
    PR655write('SS0');
end

% Do raw read
readStr = PR655rawspd(timeout);

% fprintf('Got data\n');
qual = sscanf(readStr,'%f',1);
	 
% Check for other error conditions
if qual == -1 || qual == 10
  %disp('Low light level during measurement');
  %disp('Setting returned value to zero');
  spd = zeros(S(3), 1);
elseif qual == 18 || qual == 0
	spd = PR655parsespdstr(readStr, S);
	
elseif qual ~= 0
  error('Bad return code %g from meter', qual);
end	

return