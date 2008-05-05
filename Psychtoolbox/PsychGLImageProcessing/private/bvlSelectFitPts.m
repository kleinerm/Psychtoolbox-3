function [scal] = bvlSelectFitPts(scal)
% bvlSelectFitPts
%
% Add points to the list in FITDOTLIST_L or _R. All dots are displayed on the screen
% and a circle is drawn around the circles that are in the list. The user can then select
% or deselect dots with the mouse.

% 2007-05-17 - Chris Burns
%       Major re-write of the calibration code on the Bankslab Haploscope
%       This is a complete rewrite of similar code from Ben Backus
%       (1995-97) and Jimmy Sastra(2001).  Using Matlab 7.x, Psychtoolbox 3.x
%       on Windows XP.
%       Large chunks of this are copied from the manual_calibrate.m code.
%       It would be more elegant to reuse the existing code instead of
%       copying, but was a lot of work to refactor manual_calibrate to use
%       the mouse and keyboard and PTB 3.x.  I don't want to brake that
%       code or fudge this in.  And in the interest of getting this done
%       today... shamefully copying code.



% Define colors that are used in the drawing functionsp
colorBlack      = [0    0   0   255];
colorWhite      = [255  255 255 255];
colorRed        = [255  0   0   255];
colorBlue       = [0    0   255 255];
colorGreen      = [0    255 0   255];
colorCyan       = [0    255 255 255];
colorMagenta    = [255  0   255 255];
colorYellow     = [255  255 0   255];

% indices of selected dots
indicesSelectedDots = []; 

nDots = scal.NxdotsG*scal.NydotsG;

%
% Construct 5x5 matrix grid
%
% Edges, center meridian (vert and horiz) and the midpoints

xMidPt  = (scal.NxdotsG + 1) / 2;
xDelta  = (xMidPt + 1) / 2;
xDelta  = floor(xDelta);
xIndicies = [1; xDelta; xMidPt; scal.NxdotsG+1-xDelta; scal.NxdotsG];

yMidPt  = (scal.NydotsG + 1) / 2;
yDelta  = (yMidPt + 2) / 2;
yDelta  = floor(yDelta);
yIndicies = [1; yDelta; yMidPt; scal.NydotsG+1-yDelta; scal.NydotsG];

indicesSelectedDots = [];
for xIndex = 1:length(xIndicies)
    for yIndex = 1:length(yIndicies)
        xCurr = xIndicies(xIndex);
        yCurr = yIndicies(yIndex);
        currIndex = ((xCurr - 1) * scal.NydotsG) + yCurr;
        indicesSelectedDots = [indicesSelectedDots; currIndex];
    end
end
indicesSelectedDots = 1:length(scal.XCALIBDOTS(:));
fSoftBeepVolume = 0.1;

%constants for dots and circledots
dots    = [scal.XCALIBDOTS(:)  scal.YCALIBDOTS(:)];
nDots   = length(dots(:,1));

% Define dot parameters
dotDiam         = 8;                 % Diameter of dots in pixels
selectedDotDiam = 2 * dotDiam;       % Diameter of selected dots, in pixels
dotDiamArray    = [];       % Array containing the dot diameter for each dot.
                            % This allows us to keep the dotDiam and
                            % selectedDotDiam as constants and use this
                            % array to update the diameter or selected dots
                            % on each edit.
dotColor        = colorWhite;
dotStyle        = 2;        % 0 = square pixels, 1 = circles, 2 = circles with antialiasing.

fprintf('\nbvlSelectFitPts:  Begin fitting point selection...\n');

    
% calculate screen center
xmid = scal.rect(3) / 2 - 0.5;
ymid = scal.rect(4) / 2 - 0.5;

% Enable alpha blending with proper blend-function. We need it
% for drawing of smoothed points:
Screen('BlendFunction', scal.windowPtr, GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    
% bAbort = 0;
% while ~bAbort
% 
%     % Update dot diameters
%     % Set all dots to default diameter
%     dotDiamArray = [];
%     dotDiamArray = zeros(size(dots,1), 1) + dotDiam;
%     % Update diameters for selected dots
%     dotDiamArray(indicesSelectedDots) = selectedDotDiam;
% 
%     % Fill to black
%     Screen('FillRect', scal.windowPtr, colorBlack);
% 
%     bvlDrawCrosshair(scal.windowPtr, xmid, ymid, selectedDotDiam * 2, dotColor);
%   
%     % Draw dots
%     %Screen('DrawDots', windowPtr, xy [,size] [,color] [,center] [,dot_type]);
%     Screen('DrawDots', scal.windowPtr, transpose(dots), dotDiamArray, dotColor, [0 0], dotStyle);
% 
%     % Check for input
%     [strInputName, xMouse, yMouse] = bvlWaitForInput(scal,0.010);
% 
%     % Handle specific keys we care about
%     if strcmp(strInputName, 'ESCAPE')
%         % Stop calibrating dots and return to Matlab
%         bAbort = 1;
%         Beeper('med', fSoftBeepVolume);
%     elseif strcmp(strInputName, 'LeftMouse')
%         newIndex = 0;
%         location = [xMouse yMouse]
%         dotDiffs = dots - (ones(nDots, 1) * location);
%         dotDists = sqrt(dotDiffs(:,1).^2 + dotDiffs(:,2).^2);
%         [dotDistance, newIndex] = min(dotDists);
%         if (dotDistance > dotDiam)
%             % Not close enough to a dot
%             % Signal audio tone to let the user know they need
%             % to try again
%             Beeper('low');
%             disp(dotDistance )
%             disp(dotDiam)
%         else
%             existingIndex = find(indicesSelectedDots == newIndex);
%             if isempty(existingIndex)
%                 % New dot, add it
%                 indicesSelectedDots = [indicesSelectedDots; newIndex];
%             else
%                 % The dot was previously selected, unselect it
%                 indicesSelectedDots(existingIndex) = [];
%             end
%         end
%     end
% 
%     % flip frame buffer to show all draw operations that happened in
%     % this loop.
%     Screen('Flip', scal.windowPtr);
% end


% Set timestamp to reenable mouse input
bvlAcceptCalibGuiInput(GetSecs);

% Store new selected indices
scal.FITDOTLIST         = indicesSelectedDots;
scal.FITDOTLIST_ORG     = scal.FITDOTLIST;
scal.SELECTXCALIBDOTS   = scal.XCALIBDOTS(indicesSelectedDots);
scal.SELECTYCALIBDOTS   = scal.YCALIBDOTS(indicesSelectedDots);
scal.SELECTXCALIBDOTS_ORG = scal.XCALIBDOTS_ORG(indicesSelectedDots);
scal.SELECTYCALIBDOTS_ORG = scal.YCALIBDOTS_ORG(indicesSelectedDots);
