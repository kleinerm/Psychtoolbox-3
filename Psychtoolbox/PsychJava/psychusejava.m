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
% 22.6.2025 Take Matlab R2025a and later "no Java desktop GUI" into account. (MK)

% Have usejava() and it is not a 'desktop' feature query? The Java
% 'desktop' feature is misreported as supported on Matlab R2025a and later
% as true, although R2025a and later no longer use a Java based desktop, so
% override to else on such Matlab versions, or on Octave in general, where
% there ain't a Java based desktop:
if exist('usejava', 'file') && (~strcmp(level, 'desktop') || (~IsOctave && verLessThan('matlab', '25'))) %#ok<VERLESSMATLAB>
    % usejava exists: Use it to query if Java at level 'level' is enabled.
    rc = usejava(level);
else
    % usejava does not exist: No way to find out about Java support. Assume
    % we do not have Java support.
    rc = 0;
end
