function BitsPlusGray
% BitsPlusGray(screen)
%
% Put a gray ramp in the Bits++ box.
%
% 18/04/05  ejw  Rewritten to run with OSX version of Psychtoolbox

% Define screen
whichScreen=max(Screen('Screens'));

% Find the color values which correspond to white and black.  Though on OS
% X we currently only support true color and thus, for scalar color
% arguments,
% black is always 0 and white 255, this rule is not true on other platforms will
% not remain true on OS X after we add other color depth modes.  
white=WhiteIndex(whichScreen);
black=BlackIndex(whichScreen);
gray=(white+black)/2;
if round(gray)==white
	gray=black;
end

% Open up the a window on the screen.
% This is done by calling the Screen function of the Psychophysics
% Toolbox. We need to open the onscreen window first, so that
% the offscreen storage allocated subsequently is properly
% matched to the onscreen frame buffer.
[window,screenRect] = Screen('OpenWindow',whichScreen,128,[],32,2);

% THE FOLLOWING STEP IS IMPORTANT.
% make sure the graphics card LUT is set to a linear ramp
% (else the encoded data will not be recognised by Bits++).
% There is a bug with the underlying OpenGL function, hence the scaling 0 to 255/256.  
% This demo will not work using a default gamma table in the graphics card,
% or even if you set the gamma to 1.0, due to this bug.
% This is NOT a bug with Psychtoolbox!
Screen('LoadNormalizedGammaTable',window,linspace(0,(255/256),256)'*ones(1,3));

% draw a gray background on front and back buffers
Screen('FillRect',window, gray);
Screen('Flip', window);
Screen('FillRect',window, gray);

% =================================================================
% CODE NEEDED HERE !
% "linear_lut" should be replaced here with one giving the inverse
% characteristic of the monitor.
% =================================================================
% restore the Bits++ LUT to a linear ramp
linear_lut =  repmat(round(linspace(0, 2^16 -1, 256))', 1, 3);
BitsPlusSetClut(window,linear_lut);
    
% draw a gray background on front and back buffers to clear out any old LUTs
Screen('FillRect',window, gray);
Screen('Flip', window);
Screen('FillRect',window, gray);
Screen('Flip', window);

% Close the window.
Screen('CloseAll');  
