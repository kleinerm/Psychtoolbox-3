function rc = psychusejava(level)
% rc = psychusejava(level) -- Psychtoolbox wrapper for usejava
% When running under a Matlab with support for the function 'usejava', we
% simply invoke that function and return its result.
% When running under a Matlab without that function or on Octave, we return
% zero.
%
% Drop in replacement for Matlabs usejava function.

% History:
% 20.9.2006 Written (MK)

if exist('usejava')
    % usejava exists: Use it to query if Java at level 'level' is enabled.
    rc = usejava(level);
else
    % usejava does not exist: No way to find out about Java support. Assume
    % we do not have Java support.
    rc = 0;
end
