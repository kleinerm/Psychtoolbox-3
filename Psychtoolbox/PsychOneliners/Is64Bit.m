function rc = Is64Bit
% result = Is64Bit;
%
% Returns 1 (true) if the script is running inside a 64-Bit version of
% GNU/Octave or Matlab, 0 (false) otherwise.
%

% History:
% 3.09.2012  mk  Written.
persistent rc64;

if isempty(rc64)
    rc64 = ~isempty(strfind(computer, '64')); %#ok<STREMP>
end

rc = rc64;

return;
