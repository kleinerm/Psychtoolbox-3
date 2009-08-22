function oldClut = LoadIdentityClut(windowPtr, loadOnNextFlip)
% oldClut = LoadIdentityClut(windowPtr, [loadOnNextFlip])
%
% Loads the identity clut on the windows specified by windowPtr.  If
% loadOnNextFlip is set to 1, then the clut will be loaded on the next call
% to Screen('Flip').  By default, the clut will be loaded immediately or on
% the next vertical retrace.
%
% The routine returns the old clut in 'oldClut'.
%

% History:
% ??/??/??   mk  Written.
% 05/31/08   mk  Add code to save backup copy of LUT's for later restore.

global ptb_original_gfx_cluts;


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
    
    % We start with assumption that it is a "normal" one:
    gfxhwtype = 0;
    
    % Is it a Geforce-8000 or later (G80 core or later) and is this OS/X?
    if ~isempty(strfind(winfo.GPUCoreId, 'G80')) & IsOSX %#ok<AND2>
        % Which OS/X version?
        compinfo = Screen('Computer');
        osxversion = sscanf(compinfo.system, '%*s %*s %i.%i.%i');

        % 10.5.0 or later?
        if (osxversion(1) == 10) & (osxversion(2) >= 5) & (osxversion(3) >=0) %#ok<AND2>
            % Yes. One of the releases with an embarassing amount of bugs,
            % brought to you by Apple. Need to apply an especially ugly
            % clut to force these cards into an identity mapping:
            fprintf('NVidia Geforce 8000 or later on OS/X 10.5 or later detected. Enabling special LUT hacks for totally broken operating systems.\n');
            gfxhwtype = 2;
        end
    end
else
    if ~isempty(strfind(gfxhwtype, 'ATI')) | ~isempty(strfind(gfxhwtype, 'AMD')) %#ok<OR2>
        % ATI card:
        gfxhwtype = 1;
    else
        % Unknown card: Default to NVidia behaviour:
        gfxhwtype = 0;
        warning('LoadIdentityClut: Warning! Unknown graphics hardware detected. Set up identity CLUT may be wrong!'); %#ok<WNTAG>
    end
end

% We have different CLUT setup code for the different gfxhw-vendors:

if gfxhwtype == 0
    % This works on WindowsXP with NVidia GeForce 7800 and OS/X 10.4.9 PPC
    % with GeForceFX-5200. Both are NVidia cards, so we assume this is a
    % correct setup for NVidia hardware:
    oldClut = Screen('LoadNormalizedGammaTable', windowPtr, (0:1/255:1)' * ones(1, 3), loadOnNextFlip);
end

if gfxhwtype == 1
    % This works on OS/X 10.4.9 on Intel MacBookPro with ATI Mobility
    % Radeon X1600: We assume this is the correct setup for ATI hardware:
    oldClut = Screen('LoadNormalizedGammaTable', windowPtr, ((1/256:1/256:1)' * ones(1, 3)), loadOnNextFlip);
end

if gfxhwtype == 2
    % This works on OS/X 10.5 with NVidia GeForce 8800. It is an ugly hack
    % to compensate for the absolutely horrible, embarassing bugs in Apple's NVidia
    % graphics drivers and their clut handling. Does this company still
    % have a functioning Quality control department, or are they all
    % fully occupied fiddling with their iPhones?!?

    % Sort of ok:lut = zeros(1, 1024);
    % nogood:    luti = (0:1/255:1) + 0.001;

    % better:
    luti = (1/256:1/256:1) - 0.003;
    loadlut = luti' * ones(1, 3);    
    oldClut = Screen('LoadNormalizedGammaTable', windowPtr, loadlut, loadOnNextFlip);
end

% Store backup copies of clut's for later restoration by RestoreCluts()
screenid = Screen('WindowScreenNumber', windowPtr);

% Create global clut backup cell array if it does not exist yet:
if isempty(ptb_original_gfx_cluts)
    % Create 10 slots for out up to 10 screens:
    ptb_original_gfx_cluts = cell(10,1);
end

% Do we have already a backed up original clut for 'screenid'?
% If so, we don't store this clut, as an earlier invocation of a clut
% manipulation command will have stored the really real original lut:
if isempty(ptb_original_gfx_cluts{screenid + 1})
    % Nope. Store backup:
    ptb_original_gfx_cluts{screenid + 1} = oldClut;
end

return;
