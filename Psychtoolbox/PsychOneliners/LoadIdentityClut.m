function LoadIdentityClut(windowPtr, loadOnNextFlip)
% LoadIdentityClut(windowPtr, [loadOnNextFlip])
%
% Loads the identity clut on the windows specified by windowPtr.  If
% loadOnNextFlip is set to 1, then the clut will be loaded on the next call
% to Screen('Flip').  By default, the clut will be loaded immediately or on
% the next vertical retrace.

if nargin > 2 || nargin < 1
    error('Invalid number of arguments to LoadIdentityClut.');
end

% If not specified, we'll set the clut to load right away.
if nargin == 1
    loadOnNextFlip = 0;
end

% Query what kind of graphics hardware is
% installed to drive the display corresponding to 'windowPtr':

% Query vendor of associated graphics hardware:
winfo = Screen('GetWindowInfo', windowPtr);

% We derive type of hardware and thereby our strategy from the vendor name:
gfxhwtype = winfo.GLVendor;

if ~isempty(strfind(gfxhwtype, 'NVIDIA'))
    % NVidia card:
    gfxhwtype = 0;
else
    if ~isempty(strfind(gfxhwtype, 'ATI')) | ~isempty(strfind(gfxhwtype, 'AMD'))
        % ATI card:
        gfxhwtype = 1;
    else
        % Unknown card: Default to NVidia behaviour:
        gfxhwtype = 0;
        warning('LoadIdentityClut: Warning! Unknown graphics hardware detected. Set up identity CLUT may be wrong!');
    end
end

% We have different CLUT setup code for the different gfxhw-vendors:

if gfxhwtype == 0
    % This works on WindowsXP with NVidia GeForce 7800 and OS/X 10.4.9 PPC
    % with GeForceFX-5200. Both are NVidia cards, so we assume this is a
    % correct setup for NVidia hardware:
    Screen('LoadNormalizedGammaTable', windowPtr, (0:1/255:1)' * ones(1, 3), loadOnNextFlip);
end

if gfxhwtype == 1
    % This works on OS/X 10.4.9 on Intel MacBookPro with ATI Mobility
    % Radeon X1600: We assume this is the correct setup for ATI hardware:
    Screen('LoadNormalizedGammaTable', windowPtr, ((1/256:1/256:1)' * ones(1, 3)), loadOnNextFlip);
end
