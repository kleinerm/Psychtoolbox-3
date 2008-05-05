function [scal]=NewCalibFile(scal)
%
% OLD: function NewCalibFile(GUINxdotsG, GUINydotsG, GUIMonitorSlant, XSCRSIZE, YSCRSIZE)
% function NewCalibFile(GUINxdotsG, GUINydotsG)
%
% Make a new calibration set with dots dimensions (GUINxdotsG by GUINydotsG) 
% These are specified by the user in the GUI interface menu. 
% The dots to be calibrated will uniformly spaced in a rectangle.
% 
% Input: 
%     GUINXdotsG scalar: % Number of dots, spaced at 1 cm, in x dimension.
%     GUINYdotsG scalar: % Number of dots, spaced at 1 cm, in y dimension.
%
% 06/13/01    Jimmy Sastra
% 05/01/08    Minimal fix to code (MK).

% These global variables are stored in the input file (e.g. "calibdots.mat").

% Should force odd points in new calibration interface
% MK: Applied small bugfix - this was dysfunctional:
if ~mod(scal.NxdotsG, 2)
    scal.NxdotsG = scal.NxdotsG + 1;
end
if ~mod(scal.NydotsG, 2)
    scal.NydotsG = scal.NydotsG + 1;
end

%Since array of dots will be similar for left and right screen, only one column is generated, which 
%is then used for the other screen as well
oneColumnX= [];              
oneColumnY= [];

%Generate oneColumnX and oneColumnY for XCALIBDOTS and YCALIBDOTS
for iNxdotsG = 1:scal.NxdotsG 
   oneColumnX = [oneColumnX ; iNxdotsG*scal.xStep*ones(scal.NydotsG,1)]; %#ok<AGROW>
end    
for i =  1:scal.NxdotsG 
   oneColumnY= [oneColumnY; scal.yStep*[1:scal.NydotsG]']; %#ok<NBRAK,AGROW>
end   
%Generate values for XVALUES and YVALUES

% In x-dimension
xSizeMaxLeft = round((scal.NxdotsG-1)/2);
if rem(scal.NxdotsG,2) == 1                  % If NxdotsG is uneven, range in the positive direction equals 
   xSizeMaxRight = xSizeMaxLeft;        % range in negative x-direction.            
else												 % If NxdotsG is even, range in the positive direction is 1 unit
   xSizeMaxRight = xSizeMaxLeft -1;     % smaller than range in negative direction. 
end                                     %
% In y-dimension								 % Same applies in y-direction
ySizeMaxLeft = round((scal.NydotsG-1)/2);
if rem(scal.NydotsG,2) == 1
   ySizeMaxRight = ySizeMaxLeft;   
else
   ySizeMaxRight = ySizeMaxLeft -1;
end

scal.XVALUES = [-xSizeMaxLeft:xSizeMaxRight]; %#ok<NBRAK>
scal.YVALUES = [-ySizeMaxLeft:ySizeMaxRight]; %#ok<NBRAK>

% make xy positions in centimeters that correspond to scal.XCALIBDOTS en
% scal.YCALIBDOTS
% If you want to fit a different set of values, then simply change the
% values that go into the meshgrid function (DAVID!)
[txcm, tycm] = meshgrid([scal.XVALUES],[scal.YVALUES]);
scal.xcm = reshape(txcm,numel(txcm),1);
scal.ycm = reshape(tycm,numel(tycm),1);

%Save all values in the global variables: XCALIBDOTS YCALIBDOTS XVALUES YVALUES FITDOTLIST_L FITDOTLIST_R  
scal.XCALIBDOTS = [oneColumnX]; %#ok<NBRAK>
scal.YCALIBDOTS = [oneColumnY]; %#ok<NBRAK>
scal.XCALIBDOTS_ORG = scal.XCALIBDOTS;
scal.YCALIBDOTS_ORG = scal.YCALIBDOTS;
