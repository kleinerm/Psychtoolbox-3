function rc = psychlasterror
% psychlasterror - Replacement for Matlab-7 'lasterror'.
% This is hopefully useful for older Matlab installations and
% for the Octave port:
%
% If the 'lasterror' variable, M-File or builtin function
% is supported on your Matlab installation, this function
% will return it.
%
% Otherwise, this function will return the older lasterr
% variable instead.

% exist must check for all cases, as lasterror is implemented differently
% on different Matlab versions (variable, M-File or builtin-function):
if exist('lasterror') > 0
  % Call Matlab implementation:
  rc = lasterror;
else
  % Use our simple fallback-implementation:
  rc.message = lasterr;
  rc.identifier = '';
end

return;
