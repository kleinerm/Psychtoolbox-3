function rc = psychlasterror(varargin)
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
    if nargin > 0
        try
            rc = lasterror(varargin{1});
        catch
            % This catches bugs in lasterror on Matlab 7.0 itself!
            % Doesn't recognize the 'reset' keyword!
            try
                lasterr('');
            catch
                % The climax of sadness!
            end
        end
    else
        rc = lasterror;
    end
else
    % Use our simple fallback-implementation:
    rc.message = lasterr;
    rc.identifier = '';
end

return;
