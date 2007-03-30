function ImagingStereoDemo(stereoMode)
% ImagingStereoDemo(stereoMode)
%
% Demo on how to use OpenGL-Psychtoolbox to present stereoscopic stimuli
% when the Psychtoolbox imaging pipeline is enabled. Use of the imaging
% pipeline allows for more flexible and high quality stereo display modes,
% but it requires graphics hardware with support for at least framebuffer
% objects and Shadermodel 2.0. See the Psychtoolbox Wiki about gfx-hardware
% recommendations.
%
% Press escape key to abort demo, space key to toggle modes of specific
% algorithms.
%
% 'stereoMode' specifies the type of stereo display algorithm to use:
%
% 0 == Mono display - No stereo at all.
%
% 1 == Flip frame stereo (temporally interleaved) - You'll need shutter
% glasses that are supported by the operating system, e.g., the
% CrystalEyes-Shutterglasses.
%
% 2 == Top/bottom image stereo with lefteye=top also for use with special
% CrystalEyes-hardware.
%
% 3 == Same, but with lefteye=bottom.
%
% 4 == Free fusion (lefteye=left, righteye=right)
%
% 5 == Cross fusion (lefteye=right ...)
%
% 6-9 == Different modes of anaglyph stereo for color filter glasses:
%
% 6 == Red-Green
% 7 == Green-Red
% 8 == Red-Blue
% 9 == Blue-Red
%
%
% Authors:
% Finnegan Calabro  - fcalabro@bu.edu
% Mario Kleiner     - mario.kleiner at tuebingen.mpg.de
%

inverted = 0;

if nargin < 1
    stereoMode=1;
end;

% This script calls Psychtoolbox commands available only in OpenGL-based
% versions of the Psychtoolbox. (So far, the OS X Psychtoolbox is the
% only OpenGL-base Psychtoolbox.)  The Psychtoolbox command AssertPsychOpenGL will issue
% an error message if someone tries to execute this script on a computer without
% an OpenGL Psychtoolbox
AssertOpenGL;

KbName('UnifyKeyNames');
space = KbName('space');
escape = KbName('ESCAPE');

try
    % Get the list of Screens and choose the one with the highest screen number.
    % Screen 0 is, by definition, the display with the menu bar. Often when
    % two monitors are connected the one without the menu bar is used as
    % the stimulus display.  Chosing the display with the highest dislay number is
    % a best guess about where you want the stimulus displayed.
    scrnNum = max(Screen('Screens'));
    %Screen('Preference', 'Verbosity', 6);
    
    % Windows-Hack: If mode 4 or 5 is requested, we select screen zero
    % as target screen: This will open a window that spans multiple
    % monitors on multi-display setups, which is usually what one wants
    % for this mode.
    if IsWin & (stereoMode==4 | stereoMode==5)
       scrnNum = 0;
    end

    imagingmode = kPsychNeedFastBackingStore;
    % Open double-buffered onscreen window with the requested stereo mode:
    [windowPtr, windowRect]=Screen('OpenWindow', scrnNum, BlackIndex(scrnNum), [], [], [], stereoMode, 0, imagingmode);

    % Stimulus settings:
    numDots = 1000;
    vel = 1;   % pix/frames
    dotSize = 8;
    dots = zeros(3, numDots);

    xmax = RectWidth(windowRect)/2;
    ymax = RectHeight(windowRect)/2;
    xmax = min(xmax, ymax) / 2;
    ymax = xmax;
    
    f = 4*pi/xmax;
    amp = 16;

    dots(1, :) = 2*(xmax)*rand(1, numDots) - xmax;
    dots(2, :) = 2*(ymax)*rand(1, numDots) - ymax;
    
    % Set color gains. This depends on the anaglyph mode selected:
    switch stereoMode
        case 6,
            SetAnaglyphStereoParameters('LeftGains', windowPtr,  [1.0 0.0 0.0]);
            SetAnaglyphStereoParameters('RightGains', windowPtr, [0.0 0.6 0.0]);
        case 7,
            SetAnaglyphStereoParameters('LeftGains', windowPtr,  [0.0 0.6 0.0]);
            SetAnaglyphStereoParameters('RightGains', windowPtr, [1.0 0.0 0.0]);
        case 8,
            SetAnaglyphStereoParameters('LeftGains', windowPtr, [0.4 0.0 0.0]);
            SetAnaglyphStereoParameters('RightGains', windowPtr, [0.0 0.2 0.7]);
        case 9,
            SetAnaglyphStereoParameters('LeftGains', windowPtr, [0.0 0.2 0.7]);
            SetAnaglyphStereoParameters('RightGains', windowPtr, [0.4 0.0 0.0]);
        otherwise
            %error('Unknown stereoMode specified.');
    end
    
    % Initially fill left- and right-eye image buffer with black background
    % color:
    Screen('SelectStereoDrawBuffer', windowPtr, 0);
    Screen('FillRect', windowPtr, BlackIndex(scrnNum));
    Screen('SelectStereoDrawBuffer', windowPtr, 1);
    Screen('FillRect', windowPtr, BlackIndex(scrnNum));

    % Show cleared start screen:
    Screen('Flip', windowPtr);

    % Set up alpha-blending for smooth (anti-aliased) drawing of dots:
    Screen('BlendFunction', windowPtr, 'GL_SRC_ALPHA', 'GL_ONE_MINUS_SRC_ALPHA');

    col1 = WhiteIndex(scrnNum);
    col2 = col1;
    i = 1;
    keyIsDown = 0;
    center = [0 0];
    sigma = 50;
    xvel = 2*vel*rand(1,1)-vel;
    yvel = 2*vel*rand(1,1)-vel;

    % Perform a flip to sync us to vbl and take start-timestamp in t:
    t = Screen('Flip', windowPtr);

    % Run until a key is pressed:
    while 1
        % Compute dot positions and offsets for this frame:
        center = center + [xvel yvel];
        if center(1) > xmax | center(1) < -xmax
            xvel = -xvel;
        end

        if center(2) > ymax | center(2) < -ymax
            yvel = -yvel;
        end

        dots(3, :) = -amp.*exp(-(dots(1, :) - center(1)).^2 / (2*sigma*sigma)).*exp(-(dots(2, :) - center(2)).^2 / (2*sigma*sigma));

        % Select left-eye image buffer for drawing:
        Screen('SelectStereoDrawBuffer', windowPtr, 0);
        % Draw left stim:
        Screen('DrawDots', windowPtr, dots(1:2, :) + [dots(3, :)/2; zeros(1, numDots)], dotSize, col1, [windowRect(3:4)/2], 1);

        % Select right-eye image buffer for drawing:
        Screen('SelectStereoDrawBuffer', windowPtr, 1);
        % Draw right stim:
        Screen('DrawDots', windowPtr, dots(1:2, :) - [dots(3, :)/2; zeros(1, numDots)], dotSize, col2, [windowRect(3:4)/2], 1);
        
        [pressed dummy keycode] = KbCheck;
        
        if pressed
            if keycode(space) & ismember(stereoMode, [6 7 8 9]);
                while KbCheck; end;
                inverted = 1 - inverted;
                if inverted
                    % Set inverted mode:
                    SetAnaglyphStereoParameters('InvertedMode', windowPtr);
                else
                    % Set standard mode:
                    SetAnaglyphStereoParameters('StandardMode', windowPtr);
                end
            end
            
            if keycode(escape)
                break;
            end
        end
        
        % Take timestamp of stimulus-onset after displaying the new stimulus
        % and record it in vector t:
        onset = Screen('Flip', windowPtr);
        t = [t onset];
    end

    % Done. Close the onscreen window:
    Screen('CloseAll')

    % Compute and show timing statistics:
    dt = t(2:end) - t(1:end-1);
    disp(sprintf('N.Dots\tMean (s)\tMax (s)\t%%>20ms\t%%>30ms\n'));
    disp(sprintf('%d\t%5.3f\t%5.3f\t%5.2f\t%5.2f\n', numDots, mean(dt), max(dt), sum(dt > 0.020)/length(dt), sum(dt > 0.030)/length(dt)));

    % We're done.

catch
    % Executes in case of an error: Closes onscreen window:
    Screen('CloseAll');
    psychrethrow(psychlasterror);
end;
