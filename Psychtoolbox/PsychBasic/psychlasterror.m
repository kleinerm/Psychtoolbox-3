function rc = psychlasterror
% psychlasterror - Replacement for Matlab-7 builtin 'lasterror'.
% This is hopefully useful for older Matlab installations and
% for the Octave port:
%
% If the 'lasterror' variable is supported on
% your Matlab installation, this function will return it.
%
% If your Matlab lacks this variable, this function
% will return the older lasterr variable instead.

if exist('lasterror', 'builtin') > 0
  % Call Matlab implementation:
  rc = lasterror;
else
  % Use our simple fallback-implementation:
  rc.message = lasterr;
  rc.identifier = '';
end

return;
