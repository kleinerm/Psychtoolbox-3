function VignettingCorrectionDemo(docolor, precision)
% Demonstrate how to do display devignetting aka per-pixel gain correction.
%
% Usage: VignettingCorrectionDemo([docolor=0][, precision=max]);
%
% 'docolor' if set to non-zero will compute and apply per-color channel
% gains instead of the default luminance gain.
%
% 'precision' selects the precision vs. speed tradeoff: 2 = max precision,
% min speed, 1 = balanced, 0 = max speed and minimum precision.
%
% Because gain correction is memory and compute intense, per-color
% correction is slower than global luminance only correction. Higher
% precisions for the definition of the gainmatrix are slower, and higher
% display resolutions are slower.
%
% The demo runs until a key is hit twice.
%
% On modern graphics cards, Psychtoolbox allows to automatically apply a
% luminance- or color gainfield to all presented stimuli. Each pixel
% location can get individually gain corrected. This is useful to correct
% for spatial luminance or color inhomogenities of display devices, e.g.,
% video projectors (due to lens vignetting effects), or flat panels (with
% their contrast and color view dependency).
%
% You need recent graphics hardware for this to work and to work at a
% decent speed.
%
% See "help PsychImaging" under subsection 'ColorCorrection' and "help
% PsychColorCorrection" under subsection 'GainMatrix' for explanation of
% the math, the parameters and other notable things of interest about gain
% correction.
%

% History:
% 6.3.2010  mk   Written.
%

% Assign parameters or defaults:
if nargin < 1 || isempty(docolor)
    docolor = 0;
end

if nargin < 2
    precision = [];
end

% Check for properly installed Psychtoolbox:
AssertOpenGL;

% Output to max screen:
screenid = max(Screen('Screens'));

% Use imaging pipeline for visual processing and display:
PsychImaging('PrepareConfiguration');

% Request per-pixel 2D gain correction for display:
PsychImaging('AddTask', 'FinalFormatting', 'DisplayColorCorrection', 'GainMatrix');

% Open window with gray background (128 aka 50% gray), return handle 'win':
win = PsychImaging('OpenWindow', screenid, 128);

try
    Screen('TextSize', win, 48);

    % Build a simple luminance gain matrix, the size of the window, with a
    % simple linear gradient of increasing gain values from left to right, here
    % in the gain range from 0.0 to 1.5:
    [width, height] = Screen('WindowSize', win);

    % Test support for display gain correction:
    
    % Try to read from default configuration file:
    filename = [PsychtoolboxConfigDir('ShadingCalibration') 'VignetCalibration' sprintf('_%i_%i_%i', screenid, width, height) '.mat'];
    if exist(filename, 'file')
        load(filename, 'gainMatrix');
    else
        % gainmatrix is a linear "gain gradient" from 0.0 at the left border to
        % 1.5 at the right border of the display:
        gainMatrix = (meshgrid(1:width, 1:height) / width) * 1.5;
    end
    
    % If per-colorchannel correction is requested...
    if docolor
        % Replicate linear gains into all three color gain channels...
        gainMatrix = repmat(gainMatrix, [1 1 3]);
        % ... and invert the gradient in the green channel to make it
        % visually more interesting:
        gainMatrix(:,:,2) = (1.5 - 1.5 * meshgrid(1:width, 1:height) / width);
    end
    
    % Apply gain correction matrix to display: This will take effect at the
    % next Screen('Flip'). We pass in a 'precision' flag as well, to choose
    % the tradeoff between accuracy and speed:
    PsychColorCorrection('SetGainMatrix', win, gainMatrix, [], precision);

    KbReleaseWait;
    fc = 0;
    tstart = GetSecs;
    
    % Repeat redraw loop until keypress.
    while ~KbCheck
        fc = fc + 1;
        cl = mod(fc, 170);

        % Draw a yellow oval to the screen which changes brightness periodically:
        Screen('FillOval', win, [cl cl 0]);

        % Text drawing is time consuming, btw...
        DrawFormattedText(win, 'Hit any key twice to finish!', 'center', 'center', [0 0 128]);

        % Show updated image at next refresh cycle:
        Screen('Flip', win);
    end

    % Some stats...
    fprintf('\n\nAvg redraw rate was %f Hz.\n\n', fc / (GetSecs - tstart));
    
    % Wait for keyboard stroke before closing down...
    KbStrokeWait;
        
    % Done. Close everything down and terminate:
    Screen('CloseAll');
    
catch
    % Usual error handling...
    sca;
    psychrethrow(psychlasterror);
end

return;
