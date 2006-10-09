function spd = PR650parsespdstr(readStr, S)
% spd = PR650parsespdstr(readStr,S)
%
% Parse the spectral power distribution string
% returned by the PR650.

if nargin < 2 || isempty(S)
	S = [380 5 81];
end

start = findstr(readStr,'0380.');
for k= 1:101
    %fprintf('k: %d, bi: %d, ed: %d\n', k, start+6+17*(k-1), start+6+9+17*(k-1));
    
	spd(k) = str2num(readStr(start+6+17*(k-1):start+6+9+17*(k-1)));
end

% Convert to our units standard.
spd = 4 * spd';

% Spline to desired wavelength sampling.
spd = SplineSpd([380 4 101], spd, S);
