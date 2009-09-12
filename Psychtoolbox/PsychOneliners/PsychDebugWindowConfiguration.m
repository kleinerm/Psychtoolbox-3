function PsychDebugWindowConfiguration(opaqueForHID)
% Switch PTB's onscreen window into a display mode suitable for easy debugging on single-screen setups.
%
% This function (only on OS/X) allows to setup Screen onscreen windows to be
% half-transparent, so one can simultaneously see the stimulus display and
% the Matlab window and other GUI windows.
%
% Usage: PsychDebugWindowConfiguration([opaqueForHID=0])
%
% To enable: Call PsychDebugWindowConfiguration before the Screen('OpenWindow',...) call!
% To disable: Type "clear Screen"
%
% The optional parameter 'opaqueForHID' if set to a non-zero value will
% disallow mouse clicks and other mouse actions to "get through" to the
% GUI, ie., it will make the onscreen window opaque to the mouse pointer.
%
% A setting of -1 will disable the debug mode again.
%
% Stimulus onset timing and timestamping will be inaccurate in this mode
% and graphics performance will be reduced! Don't use for timing tests or
% during real experiment sessions!
%

% History:
% 30.7.2009 mk  Written.

if nargin < 1
    opaqueForHID = 0;
end

if IsOSX | IsWin %#ok<OR2>
    if IsOSX
        % Disable high precision timestamping:
        Screen('Preference', 'VBLTimestampingMode', -1);

        % Skip sync tests:
        Screen('Preference', 'SkipSyncTests', 2);

        % Use AGL + Carbon, even for fullscreen windows:
        oldconserve = Screen('Preference', 'ConserveVRAM');
        Screen('Preference', 'ConserveVRAM', bitor(oldconserve, 16384));
    end

    if opaqueForHID
        % Set windows to be transparent, but not for mouse and keyboard:
        Screen('Preference', 'WindowShieldingLevel', 1750);
    else
        % Set windows to be transparent, also for mouse and keyboard:
        Screen('Preference', 'WindowShieldingLevel', 1250);
    end
	
	if opaqueForHID == -1
        % Set windows to be normal, i.e., completely opaque:
        Screen('Preference', 'WindowShieldingLevel', 2000);
	end
end

return;
