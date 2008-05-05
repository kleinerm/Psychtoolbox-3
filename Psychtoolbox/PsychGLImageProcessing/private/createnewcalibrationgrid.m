function [scal] = createnewcalibrationgrid(scal)
% adjustable parameters:
% the number of rows and columns of dots. These will be evenly spaced
% across the display in a rectangular grid.
%
% bugs: getmouse does not return right x and y values. It may therefore
% make it a little hard to select the right points in the grid.

% MK: Set in calling routine, or overriden there by user input:
% scal.NxdotsG = 37;
% scal.NydotsG = 27;

% MK: Done in calling routine...
% % determine the arrangement of monitors, if more than one monitor is used
% scrns = Screen('Screens');
% 
% % decide what monitor to calibrate. This won't be asked if there is only
% % one monitor
% if length(scrns)>1, 
%     if isempty(varargin),
%         screenNumber = str2double(input(['Screen number of display to calibrate? [' num2str(scrns) '] '],'s'));
%     else
%         screenNumber = varargin{1};
%     end
% else
%     screenNumber = scrns;
% end
% 
% if ~IsOctave
%     commandwindow;
%     pause(.5);
% end
% 
% % all information concerning the calibration is put into the structure
% % scal. This structure will contain: 
% % screen properties
% scal.screenNumber = screenNumber;
% [scal.windowPtr,scal.rect] = Screen('OpenWindow', screenNumber);        % windowPtr: psychtoolbox reference to the active drawing window, % rect: resolution of the active drawing window in pixels [x(1) x(end) y(1) y(end)]

% all dots
scal.XCALIBDOTS = []; scal.YCALIBDOTS = [];                             % the x and y coordinates of all dots in pixels
scal.XCALIBDOTS_ORG = []; scal.YCALIBDOTS_ORG = [];                     % the x and y coordinates of all dots at their initial position in pixels
scal.XVALUES = []; scal.YVALUES = [];                                   % probably, these can be removed.
scal.xStep = RectWidth(scal.rect)/(scal.NxdotsG + 1);                   % distance between dots in x-direction in pixels
scal.yStep = RectHeight(scal.rect)/(scal.NydotsG + 1);                  % distance between dots in y-direction in pixels
scal.nDots = scal.NxdotsG*scal.NydotsG;                                 % total number of dots on the display
scal.xcm = []; scal.ycm = [];                                           % positions of reference objects (hair crossings in case of the loom) the real world coordinates

% dot selection
scal.FITDOTLIST = [];                                                   % indices to the dot selection (index for 'dots' variable that is composed of XCALIBDOTS and YCALIBDOTS (without any further adjustment))
scal.SELECTXCALIBDOTS = []; scal.SELECTYCALIBDOTS  = [];                % the x and y coordinates of the dot selection in pixels
scal.SELECTXCALIBDOTS_ORG = []; scal.SELECTYCALIBDOTS_ORG = [];         % the initial x and y coordinates of the dot selection in pixels (so before any adjustments)

% set the Nth order polynomial
scal.NOrderPoly = 3; % this must be three. Higher order ones cannot be used yet

% start creating the calibration grid
scal = NewCalibFile(scal);                                              % calculate what the positions of the dots in pixels

scal.isDONTSTOP = 1; % only create calibration grid. Don't allow input from user. Continue to next step in calibraiton

scal = manual_calibrate(scal);  % This function will update the XCALIBDOTS en YCALIBDOTS (positions of the dots in pixels). It needs an open onscreen window!
scal.isDONTSTOP = 0;

% MK: Disabled...
% eval(['save currentCalib[' num2str(scal.screenNumber) '].mat']); % save the data, just to be sure
% Screen('CloseAll');
% ShowCursor;
