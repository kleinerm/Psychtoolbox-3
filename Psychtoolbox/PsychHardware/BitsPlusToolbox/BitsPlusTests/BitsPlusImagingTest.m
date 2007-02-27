% BitsPlusImagingTest
%
% Simple test of Bits++ interface in normal mode.  Writes CLUT
% frame buffer, so this is how Bits++ should be configured to
% accept it.
%
% This test routine employs the PTB imaging pipeline to do the
% job.
%
% 02/24/07 Derived from BitsPlusSimpleTest (MK).

% Define screen
whichScreen = max(Screen('Screens'));

% Increase level of verbosity so we get all the debug-messages:
Screen('Preference', 'Verbosity', 5);

% Open window fill frame buffer with zeros
[window, screenRect] = Screen('OpenWindow', whichScreen, 0);

% First load the graphics hardwares gamma table with an identity mapping,
% so it doesn't interfere with Bits++
LoadIdentityClut(window);

% Now enable finalizer hook chains and load them with the special Bits++
% command. 
Screen('HookFunction', window, 'PrependBuiltin', 'LeftFinalizerBlitChain', 'Builtin:RenderClutBits++', ''); 
Screen('HookFunction', window, 'Enable', 'LeftFinalizerBlitChain');

% This only on quad-buffered stereo contexts:
%Screen('HookFunction', window, 'PrependBuiltin', 'RightFinalizerBlitChain', 'Builtin:RenderClutBits++', ''); 
%Screen('HookFunction', window, 'Enable', 'RightFinalizerBlitChain');

% Use BITS++ to set uniform lookup tables of increasing values.
% Hit key to proceed through. Screen intensity should increase
% monotonically.
for colorval = linspace(0.0, 1.0, 2000)
	uniclut = colorval*ones(256,3);
	fprintf('Setting to value %g\n',colorval);
    % The setting 2 means: Don't load hardware gamma table, but just
    % store clut for later use by the special Bits++ blitter at
    % Screen('Flip') time.
    Screen('LoadNormalizedGammaTable', window, uniclut, 2);

    % Show it. Shows stimulus and updates CLUT by drawing the T-Lock stuff
    % into top left corner of display.
    Screen('Flip', window);

    %KbWait;
    %while KbCheck; end;
end

KbWait;

% Disable Bits++ encoders: This will revert the behaviour of CLUT handling
% to normal...
Screen('HookFunction', window, 'Disable', 'LeftFinalizerBlitChain');
Screen('HookFunction', window, 'Disable', 'RightFinalizerBlitChain');

% Close the window.
Screen(window,'Close');

% Blank the screen
BitsPlusBlank(whichScreen);
