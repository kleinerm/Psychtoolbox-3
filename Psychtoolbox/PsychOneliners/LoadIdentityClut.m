function LoadIdentityClut(windowPtr, loadOnNextFlip)
% LoadIdentityClut(windowPtr, [loadOnNextFlip])
%
% Loads the identity clut on the windows specified by windowPtr.  If
% loadOnNextFlip is set to , then the clut will be loaded on the next call
% to Screen('Flip').  By default, the clut will be loaded immediately or on
% the next vertical retrace.

if nargin > 2 || nargin < 1
    error('Invalid number of arguments to LoadIdentityClut.');
end

% If not specified, we'll set the clut to load right away.
if nargin == 1
    loadOnNextFlip = 0;
end

Screen('LoadNormalizedGammaTable', windowPtr, (0:1/255:1)' * ones(1, 3), loadOnNextFlip);
