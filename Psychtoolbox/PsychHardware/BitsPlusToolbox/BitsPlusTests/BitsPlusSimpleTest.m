function BitsPlusSimpleTest(whichScreen)
% BitsPlusSimpleTest([whichScreen])
%
% Simple test of Bits++ interface in normal mode.  Writes CLUT
% frame buffer, so this is how Bits++ should be configured to
% accept it.
%
% If the optional 'whichScreen' screen index of the Bits++ screen is left
% out, the script will choose the display with the highest screen id.
%
% If everything works correctly, you should see the following:
%
% 1. First some random color "junk" on the Bits++ display for 3 seconds.
% 2. Then a black screen.
% 3. Then, on each keystroke, an increase in display luminance.
% 4. After twenty keystrokes, a fully white display.
% 5. After another keystroke, the Bits++ should revert to a normal desktop
%    display.

% 9/13/02 dhb
% 2/26/03 dhb  Force on-card CLUT to be a identity mapping.
% 9/20/03 dhb  No more calls to MEX file.
% 3/11/08 mk   Random improvements to procedure.

% Define screen:
if nargin < 1
    whichScreen = max(Screen('Screens'));
end

% Get current gfx-card lut, so we can restore it later:
oldgfxlut = Screen('ReadNormalizedGammatable', whichScreen);

% Load nonlinear table, just to make sure we start with an unsuitable clut:
junkLut = repmat(([1:256]'/256).^0.7, 1, 3);
Screen('LoadNormalizedGammatable', whichScreen, junkLut);

% Hold it for 3 seconds:
WaitSecs(3);

% Open window fill frame buffer with zeros
window = Screen('OpenWindow', whichScreen, 0, []);

% Load an identity clut into the gfx-card so T-Lock code works:
LoadIdentityClut(window);

% Use BITS++ to set uniform lookup tables of increasing values.
% Hit key to proceed through. Screen intensity should increase
% monotonically.
for colorval = round(linspace(0,2^16-1,20))
	uniclut = colorval*ones(256,3);
	fprintf('Setting to value %g   -- Press any key to continue.\n',colorval);
	BitsPlusSetClut(window, uniclut);
	KbStrokeWait;
end

% Restore linear CLUT on the Bits++
linear_lut = repmat(linspace(0, 2^16-1, 256)', 1, 3);
BitsPlusSetClut(window, linear_lut);

% Close the window.
Screen('CloseAll');

% Restore original gfx-luts:
Screen('LoadNormalizedGammatable', whichScreen, oldgfxlut);

return;
