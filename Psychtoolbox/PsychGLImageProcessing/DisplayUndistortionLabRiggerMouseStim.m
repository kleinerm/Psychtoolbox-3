function scal = DisplayUndistortionLabRiggerMouseStim(caliboutfilename, screenid)
% scal = DisplayUndistortionLabRiggerMouseStim(caliboutfilename[, screenid])
%
% Hi-Def TV for mice - Courtesy of Labrigger.
%
% CAUTION: This is not a finished, polished routine, but a template for
% actual producers of TV for mice to get started. Make sure you understand
% this code before using it!
%
% Geometric display calibration procedure for undistortion of distorted
% displays. Needs graphics hardware with basic support for the PTB imaging
% pipeline.
%
% This one is an example of how to integrate sample code from LabRigger
% for visual stimulation of a mouse with a single flat screen. It does the
% neccessary undistortion. For explanation and detail go to the original
% code and method presented at:
%
% http://labrigger.com/blog/2012/03/06/mouse-visual-stim/comment-page-1/
%
% Please note this is quickly hacked together throwaway sample code. It
% almost literally copy and pasted the code from LabRigger into the
% subroutine MouseStim() and then converts its output calibration matrices
% into a calibration file useable by Psychtoolbox DisplayUndistortionCSV
% method, so these undistortions can be applied to live visual stimuli
% fast and in realtime. All display and stimulation setup parameters are
% hard-coded in MouseStim(). Somebody should really clean up this routine
% and test it on a real stimulation setup. If you've successfully done so,
% please contribute the enhanced version of this M-File back to the PTB
% for integration into future releases.
%
% General boiler-plate description of working and use:
%
% Psychtoolbox can "undistort" your visual stimuli for you: At stimulus
% onset time, PTB applies a geometric warping transformation to your
% stimulus which is meant to counteract or cancel out the geometric
% distortion caused by your display device. If both, PTB's warp transform
% and the implicit distortion transform of the display match, your stimulus
% will show up undistorted on the display device.
%
% For this to work, PTB needs a non-ancient graphics card with support for
% the PTB imaging pipeline. All ATI/AMD cards starting with Radeon 9500 and
% all NVidia cards of type GeForce-FX5200 and later, as well as the Intel-GMA 950
% and later should be able to do it, although more recent cards will have a higher
% performance.
%
% DisplayUndistortionLabRiggerMouseStim defines a continous mapping
% (x', y') = f(x, y) from uncorrected input pixel locations (x,y) in
% your stimulus image to output locations (x', y') on your display.
% This mapping is defined by a linear mesh of quadrilaterals, as computed
% in the subfunction MouseStim().
%
% How to use:
% -----------
%
% Execute the function with the following parameters:
%
% `caliboutfilename` Name of the file to which calibration results should
% be stored. If no name is provided, the file will be stored inside the
% 'GeometryCalibration' subfolder of your Psychtoolbox configuration
% directory (path is PsychToolboxConfigDir('GeometryCalibration'). The
% filename will contain the screenid of the display that was calibrated.
%
% `screenid` screen id of the target display for calibration. The parameter
% is optional, defaults to zero, and is only used to generate the default
% filename for the output file.
%
% This script will print out a little snippet of code that you can paste
% and include into your experiment script - That will automatically load
% the calibration result file and apply the proper undistortion operation.
%
% You can see an example use of it in ImagingVideoCaptureDemo.m
%
% A quick way to test your calibration created with this script is to
% call ImageUndistortionDemo (caliboutfilename, 'checkerboard'). However,
% for production use you'd rather use your calibration via PsychImaging,
% as shown in ImagingVideoCaptureDemo or the code snippet printed by this
% function.

% History:
%
% 24-Jul-2015  mk  Written, based on DisplayUndistortionCSV.

% Setup defaults:
PsychDefaultSetup(0);

if ~exist('screenid', 'var') || isempty(screenid)
  screenid = 0;
end

% At this point, screenid contains the final screenid for the screen to
% calibrate. Assign it to scal struct. This will create 'scal' if it
% doesn't exist yet, or override its screenid in some cases:
scal.screenNumber = screenid;

% Define type of mapping for this calibration method: This is used in the
% CreateDisplayWarp() routine when parsing the calibration file to detect
% the type of undistortion method to use, ie. how to interpret the data in
% the calibration file to setup the calibration.
warptype = 'CSVDisplayList'; %#ok<NASGU>

% We won't use normalized coordinates, but absolute pixel coordinates for
% this method: Encoded source and target coordinates are absolute locations
% in units of pixels
scal.useUnitDisplayCoords = 0;

% Compute mapping matrices for the "Mouse visual stimulus" method presented/developed
% by LabRigger under: http://labrigger.com/blog/2012/03/06/mouse-visual-stim/comment-page-1/
%
% Use a subsampling of the dense calibration matrices of 10. Only every 10th
% pixel in x and y direction is used to define the warpmesh, so we cut down
% the amount of geometry to process to 1 / (10*10) = 1/100 th. Bilinear
% interpolation is used for intermediate pixel locations. Tweak the subdivision
% value to your needs aka your calibration accuracy <-> performance tradeoff.
[xi, yi, xS, yS] = MouseStim(10);

% Build 2D source and destination matrices: rows x cols per plane,
% 2 planes for x and y components:
rows = size(xi, 1);
cols = size(xi, 2);

% Vertex coordinates of the rendered output mesh quad vertices:
scal.vcoords = zeros(rows, cols, 2);

% Corresponding texture coordinates for sourcing from user provided input
% image framebuffer:
scal.tcoords = zeros(rows, cols, 2);

% Assign from output of the Labrigger MouseStim implementation
scal.vcoords(:,:,1) = xi;
scal.vcoords(:,:,2) = yi;
scal.tcoords(:,:,1) = xS;
scal.tcoords(:,:,2) = yS;

% 'scal' contains the final results of calibration. Write it out to
% calibfile for later use by the runtime routines:

% Check if name for calibration result file is provided:
if ~exist('caliboutfilename', 'var')
  caliboutfilename = [];
end

if isempty(caliboutfilename)
  % Nope: Assign default name - Store in dedicated subfolder of users PTB
  % config dir, with a well defined name that also encodes the screenid
  % for which to calibrate:
  caliboutfilename = [ PsychtoolboxConfigDir('GeometryCalibration') 'CSVCalibdata' sprintf('_%i', screenid) '.mat'];
  fprintf('\nNo name for calibration file provided. Using default name and location...\n');
end

% Print name of calibfile and check for existence of file:
fprintf('Name of calibration result file: %s\n\n', caliboutfilename);
if exist(caliboutfilename, 'file')
  answer = input('This file already exists. Overwrite it [y/n]? ','s');
  if ~strcmpi(answer, 'y')
    fprintf('\n\nCalibration aborted. Please choose a different name for calibration result file.\n\n');
    return;
  end
end

% Save all relevant calibration variables to file 'caliboutfilename'. This
% method should work on both, Matlab 6.x, 7.x, ... and GNU/Octave - create
% files that are readable by all runtime environments:
save(caliboutfilename, 'warptype', 'scal', '-mat', '-V6');

fprintf('Creation of Calibration file finished :-)\n\n');
fprintf('You can apply the calibration in your experiment script by replacing your \n')
fprintf('win = Screen(''OpenWindow'', ...); command by the following sequence of \n');
fprintf('commands:\n\n');
fprintf('PsychImaging(''PrepareConfiguration'');\n');
fprintf('PsychImaging(''AddTask'', ''LeftView'', ''GeometryCorrection'', ''%s'');\n', caliboutfilename);
fprintf('win = PsychImaging(''OpenWindow'', ...);\n\n');
fprintf('This would apply the calibration to the left-eye display of a stereo setup.\n');
fprintf('Additional options would be ''RightView'' for the right-eye display of a stereo setup,\n');
fprintf('or ''AllViews'' for both views of a stereo setup or the single display of a mono\n');
fprintf('setup.\n\n');
fprintf('The ''GeometryCorrection'' call has a ''debug'' flag as an additional optional parameter.\n');
fprintf('Set it to a non-zero value for diagnostic output at runtime.\n');
fprintf('E.g., PsychImaging(''AddTask'', ''LeftView'', ''GeometryCorrection'', ''%s'', 1);\n', caliboutfilename);
fprintf('would provide some debug output when actually using the calibration at runtime.\n\n\n');

% Done.
return;

end

% MouseStim is mostly a verbatim copy of the sample code on LabRigger,
% with some modifications to make it work for PTB.
function [xi, yi, xS, yS] = MouseStim(subdivide)
  close all;

  % Monitor size and position variables
  w = 56.69;  % width of screen, in cm
  h = 34.29;  % height of screen, in cm
  cx = w/2;   % eye x location, in cm
  cy = 11.42; % eye y location, in cm

  % Distance to bottom of screen, along the horizontal eye line
  zdistBottom = 24.49;     % in cm
  zdistTop    = 14.18;     % in cm

  % Alternatively, you can specify the angle of the screen
  %screenAngle = 72.5;   % in degrees, measured from table surface in front of screen to plane of screen
  %zdistTop = zdistBottom - (h*sin(deg2rad(90-screenAngle)));

  %pxXmax = 200; % number of pixels in an image that fills the whole screen, x
  %pxYmax = 150; % number of pixels in an image that fills the whole screen, y

  % MK: Use bigger input/output framebuffer of a 1680 x 1050 flat panel.
  % We add 1 pixel in size, so we don't get cutoff at the bottom and right
  % border if 'subdivide' is > 1.
  pxXmax = 1681; % number of pixels in an image that fills the whole screen, x
  pxYmax = 1051; % number of pixels in an image that fills the whole screen, y

  % Internal conversions
  top = h-cy;
  bottom = -cy;
  right = cx;
  left = cx - w;

  % Convert Cartesian to spherical coord
  % In image space, x and y are width and height of monitor and z is the
  % distance from the eye. I want Theta to correspond to azimuth and Phi to
  % correspond to elevation, but these are measured from the x-axis and x-y
  % plane, respectively. So I need to exchange the axes this way, prior to
  % converting to spherical coordinates:
  % orig (image) -> for conversion to spherical coords
  % Z -> X
  % X -> Y
  % Y -> Z

  [xi,yi] = meshgrid(1:pxXmax,1:pxYmax);

  % MK: Need to shift - 1 because OpenGL coordinates are 0-based, not 1-based
  % as Matlabs, otherwise we'd get artifacts at the top-left corner of the
  % display due to omitted data:
  xi = xi - 1;
  yi = yi - 1;

  cart_pointsX = left + (w/pxXmax).*xi;
  cart_pointsY = top - (h/pxYmax).*yi;
  cart_pointsZ = zdistTop + ((zdistBottom-zdistTop)/pxYmax).*yi;
  [sphr_pointsTh sphr_pointsPh sphr_pointsR] ...
              = cart2sph(cart_pointsZ,cart_pointsX,cart_pointsY);

  % view results
  figure
  subplot(3,2,1)
  imagesc(cart_pointsX);
  colorbar
  title('image/cart coords, x')
  subplot(3,2,3)
  imagesc(cart_pointsY);
  colorbar
  title('image/cart coords, y')
  subplot(3,2,5)
  imagesc(cart_pointsZ);
  colorbar
  title('image/cart coords, z')

  subplot(3,2,2)
  imagesc(rad2deg(sphr_pointsTh));
  colorbar
  title('mouse/sph coords, theta')
  subplot(3,2,4)
  imagesc(rad2deg(sphr_pointsPh));
  colorbar
  title('mouse/sph coords, phi')
  subplot(3,2,6)
  imagesc(sphr_pointsR);
  colorbar
  title('mouse/sph coords, radius')

  % Rescale the Cartesian maps into dimensions of radians
  xmaxRad = max(sphr_pointsTh(:));
  ymaxRad = max(sphr_pointsPh(:));

  fx = xmaxRad/max(cart_pointsX(:));
  fy = ymaxRad/max(cart_pointsY(:));

  % Compute matrices with sampling positions, needed for Psychtoolbox:
  xS = interp2(cart_pointsX.*fx,cart_pointsY.*fy,xi,sphr_pointsTh,sphr_pointsPh);
  yS = interp2(cart_pointsX.*fx,cart_pointsY.*fy,yi,sphr_pointsTh,sphr_pointsPh);

  h = figure;
  subplot(1,2,1);
  imagesc(xS);
  colorbar
  title('Lookup position input x:')
  subplot(1,2,2);
  imagesc(yS);
  colorbar
  title('Lookup position input y:')

  % Subsample to only use every subdivide'th sample:
  xi = xi(1:subdivide:end, 1:subdivide:end);
  yi = yi(1:subdivide:end, 1:subdivide:end);

  xS = xS(1:subdivide:end, 1:subdivide:end);
  yS = yS(1:subdivide:end, 1:subdivide:end);

  % We are done with creating output useable for Psychtoolbox.

  %% And here’s the debug code to try the distortion out in Matlab/Octave:
  if 1
    %% try a distortion

    % make source image
    checkSize = 105; % pixels per side of each check
    w = 1680; % width, in pixels
    h = 1050; % height, in pixels
    I = double(checkerboard(checkSize,round(h/checkSize/2),round(w/checkSize/2))>0.5);

    % alternate source image
    %I = zeros(150*4,200*4);
    %I(105*4:125*4,:)=0.2;
    %I(20*4:40*4,:)=0.4;

    if isequal(size(I), size(xi))
      % Test apply the distortion via interpolation and plotting in Matlab:
      ZI = interp2(cart_pointsX.*fx,cart_pointsY.*fy,I,sphr_pointsTh,sphr_pointsPh);
      h=figure;
      subplot(1,2,1);
      imshow(I);
      subplot(1,2,2);
      imshow(ZI);
    end
  end

  % Done.
end

% Inline replacement for missing rad2deg() in default Octave 3.8 installation:
function deg = rad2deg(rad)
  deg = rad * 180 / pi;
end
