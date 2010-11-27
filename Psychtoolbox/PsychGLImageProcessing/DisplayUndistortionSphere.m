function DisplayUndistortionSphere(varargin)
% Create geometric display calibration file for projection onto a sphere.
%
% CAUTION: This function isn't properly implemented yet and will likely
% not work for you as expected. Stay tuned...
%
% This is just a wrapper around DisplayUndistortionHalfCylinder() which
% does the actual work for both half-cylindrical and half-spherical
% projection screens.
%

fprintf('I am calling DisplayUndistortionHalfCylinder() now. Please answer\n');
fprintf('the following question by typing an "s" for spherical correction.\n');
DisplayUndistortionHalfCylinder(varargin{:});

return;
