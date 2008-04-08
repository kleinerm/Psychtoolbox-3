function BitsPlusImagingTest(whichScreen)
% BitsPlusImagingTest([whichScreen])
%
% Simple test of Bits++ interface in normal mode.  Writes CLUT
% frame buffer, so this is how Bits++ should be configured to
% accept it.
%
% This test routine employs the PTB imaging pipeline to do the
% job.
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

% History:
% 02/24/07 Derived from BitsPlusSimpleTest (MK).
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

% Open window in Bits++ mode, fill frame buffer with zeros:
window = BitsPlusPlus('OpenWindowBits++', whichScreen, 0);

% Use BITS++ to set uniform lookup tables of increasing values.
% Hit key to proceed through. Screen intensity should increase
% monotonically.
for colorval = linspace(0.0, 1.0, 20)
	uniclut = colorval*ones(256,3);
	fprintf('Setting to value %g   -- Press any key to continue.\n',colorval);
    % The setting 2 means: Don't load hardware gamma table, but just
    % store clut for later use by the special Bits++ blitter at
    % Screen('Flip') time.
    Screen('LoadNormalizedGammaTable', window, uniclut, 2);

    Screen('FillRect', window, [255 255 0], [0 0 400 400]);
    
    % Show it. Shows stimulus and updates CLUT by drawing the T-Lock stuff
    % into top left corner of display.
    Screen('Flip', window);

    % Wait for keypress:
    KbStrokeWait;
end

% Wait a last time:
KbStrokeWait;

% Restore Bits++ Identity CLUT so it can be used as normal display:
BitsPlusPlus('LoadIdentityClut', window);
Screen('Flip', window);

% Close the window.
Screen('CloseAll');

% Restore original gfx-luts:
Screen('LoadNormalizedGammatable', whichScreen, oldgfxlut);

return;
