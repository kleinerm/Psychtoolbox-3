function M = PsychCamIntrinsicsToGLProjectionMatrix(cx, cy, fx, fy, zNear, zFar)
% M = PsychCamIntrinsicsToGLProjectionMatrix(cx, cy, fx, fy, zNear, zFar)
%
% Given camera intrinsic parameters from some camera calibration software, and
% the desired zNear and zFar clipping planes of the view frustum, compute and
% return a projection matrix for use as GL_PROJECTION_MATRIX for OpenGL rendering
% with a virtual pinhole camera that approximates the properties of the real camera.
%
% Parameters:
%
% cx = Optical sensor center in pixels.
% cy = Optical sensor center in pixels.
% fx = Focal length in units of pixels in x direction.
% fy = Focal length in units of pixels in y direction.
% zNear = Near clipping plane.
% zFar = Far clipping plane.
%
% Returns the 4-by-4 GL_PROJECTION_MATRIX.
%

% History:
% 15-Mar-2024  mk   Written.

if nargin ~= 6
    error('Invalid number of parameters, must be all six.');
end

if cx <= 0 || cy <= 0 || fx <= 0 || fy <= 0 || zNear <=0 || zFar <= 0
    error('One of cx, cy, fx, fy, zNear, zFar is not greater than zero, as required.');
end

if zFar <= zNear
    error('zNear is not smaller than zFar, as required.');
end


M = reshape([ fx / cx, 0, 0, 0, 0, fy / cy, 0, 0, 0, 0, -(zFar + zNear) / (zFar - zNear), ...
            -1, 0, 0, -2 * zFar * zNear / (zFar - zNear), 0], 4, 4);

return;
