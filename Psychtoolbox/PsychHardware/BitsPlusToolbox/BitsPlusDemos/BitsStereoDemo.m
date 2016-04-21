function BitsStereoDemo
% BitsStereoDemo
%
% Demonstration of displaying a precalculated texture based movie, whilst
% controlling the FE1 goggles using a Tlock packet.
%
% This demo needs the Bits++ or Bits# from Cambridge Research Systems!
%
% 26/04/2005  ejw  Wrote it.
% 21/04/2016  mk   Rewritten to use modern PTB functions.

% Startup checks, KbName works identical on all operating systems:
PsychDefaultSetup(1);

% Define screen to use:
whichScreen = max(Screen('Screens'));

% Get color index values for black and 50% gray:
black = BlackIndex(whichScreen);
gray = GrayIndex(whichScreen);

% Open up a window on the screen for use in Bits++ mode on a
% CRS Bits+, Bits# etc. This will also load an identity gamma
% ramp into the Bits device. Ask for stereomode 1 = frame-sequential
% stereo:
PsychImaging('PrepareConfiguration');
PsychImaging('AddTask', 'General', 'EnableBits++Bits++Output');
window = PsychImaging('OpenWindow', whichScreen, gray, [], [], [], 1);

% Switch a Bits# display device into Bits++ mode. On a older Bits+
% this will do nothing, as that one needs to be configured manually:
BitsPlusPlus('SwitchToBits++');

% Enable FE1 stereo goggles connected to the Bits device:
BitsPlusPlus('UseFE1StereoGoggles', window, 1);

% =================================================================
% CODE NEEDED HERE !
% "linearizing_lut" should be replaced here with one giving the inverse
% characteristic of the monitor.
% =================================================================
% restore the Bits++ LUT to a linear ramp
linearizing_lut = repmat(linspace(0, 1, 256)', 1, 3);
Screen('LoadNormalizedGammaTable', window, linearizing_lut, 2);

texture_size = 256;
[x,y] = meshgrid(1: texture_size, 1: texture_size);

% number of cycles of sine wave in patch
cycles=10;
for orient=1:180
    phase=cycles*2*pi*((sin(pi*(orient-1)/180)*(y - texture_size/2)+cos(pi*(orient-1)/180)*(x - texture_size/2)) - 1)/(texture_size/2);
    mono=gray*(sin(phase)+1);
    left(orient) = Screen('MakeTexture', window, mono);

    phase=cycles*2*pi*((-sin(pi*(orient-1)/180)*(y - texture_size/2)+cos(pi*(orient-1)/180)*(x - texture_size/2)) - 1)/(texture_size/2);
    mono=gray*(sin(phase)+1);
    right(orient) = Screen('MakeTexture', window, mono);
end

fprintf('Displaying counter-rotating gratings, hold a key to exit \n');
KbReleaseWait;

% Run until key press:
while ~KbCheck
    for orient = 1:180
        % Left eye image:
        Screen('SelectStereoDrawBuffer', window, 0);
        Screen('DrawTexture', window, left(orient));
        Screen('DrawText', window, 'Displaying counter-rotating gratings, hold a key to exit.', 30, 30, 0);

        % Right eye image:
        Screen('SelectStereoDrawBuffer', window, 1, 0);
        Screen('DrawTexture', window, right(orient));

        % Show them:
        Screen('Flip', window);

        if KbCheck
            break;
        end
    end
end

% Release all textures:
Screen('Close');

% Disable FE1 stereo goggles connected to the Bits device:
BitsPlusPlus('UseFE1StereoGoggles', window, 0);

% Not sure if this makes sense, as it would bring back the FE1
% goggles into an undefined state? But it was there in the original
% demo, so we'll leave it for now:
%
if 1
    % Reset the digital output pins at the next flip:
    Mask = 0;
    Command = 0;
    Data = zeros(1,248);
    BitsPlusPlus('DIOCommand', window, 1, Mask, Data, Command);
end

% If the system only has one screen, set the LUT in Bits++ to a linear ramp
% if the system has two or more screens, then blank the screen.
if whichScreen == 0
    % Restore the Bits++ LUT to a linear ramp:
    BitsPlusPlus('LoadIdentityClut', window);
else
    % Blank the screen:
    Screen('LoadNormalizedGammaTable', window, zeros(256,3), 2);
end

% This flip applies the LUT update and the DIOCommand scheduled above:
Screen('Flip', window);

% Close the window.
Screen('CloseAll');

% We are done.
return;
