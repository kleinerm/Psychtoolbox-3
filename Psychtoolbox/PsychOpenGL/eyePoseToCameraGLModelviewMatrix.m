function [modelview, eyePoseT] = eyePoseToCameraGLModelviewMatrix(eyePose, eyeLocalTranslate)
% eyePoseToCameraGLModelviewMatrix() - Convert eyePose directly to OpenGL modelview matrix.
%
% Usage:
%
% modelview = eyePoseToCameraGLModelviewMatrix(eyePose [, eyeLocalTranslate])
%
% Input arguments:
%
% 'eyePose' is a [tx, ty, tz, rx, ry, rz, rw] vector, with the first 3 components
% defining eye translation, and the last 4 components defining a rotation Quaternion
% that defines eye orientation.
%
% 'eyeLocalTranslate' is an optional 3 component translation vector that gets applied
% to 'eyePose' position, but within the eyes own rotated local coordinate system.
% This is useful if 'eyePose' is not actually describing an eye pose, but the tracked
% global head pose or global HMD pose. Applying suitable 'eyeLocalTranslate' translation
% vectors allows to derive the eyes position from the head/HMD position.
%
% Return arguments:
%
% 'modelview' is a 4x4 matrix that can be directly loaded into OpenGL as GL_MODELVIEW
% matrix, e.g., glLoadMatrixd(modelview); to define camera position and orientation
% for rendering accordign to eyePose. Iow. modelview is already the inverse matrix of
% eyePose.
%
% 'eyePoseT' is the original input 'eyePose', but optionally transformed by 'eyeLocalTranslate'.
% It is not the inverse of 'eyePos', like 'modelview'!

% History:
% 14-Sep-2015  mk  Written.

% Extract tanslation and rotation quaternion components:
translation = eyePose(1:3);
orientationQ = eyePose(4:7);

if nargin >= 2 && ~isempty(eyeLocalTranslate)
  % Apply translation in the local rotated coordinate frame, e.g., if eyePose
  % is actually a head pose and we need to translate from head rotation center to
  % rotated eyes:
  translation = translation + (qGetR(orientationQ) * eyeLocalTranslate')';
end

% Compute inverse of new pose and convert into OpenGL 4x4 right-handed reference
% frame modelview matrix, ie. a matrix that can be used to simulate a camera
% corresponding to the eye pose:
orientationQInverse = [-orientationQ(1:3), orientationQ(4)];
R = diag([1,1,1,1]);
R(1:3, 1:3) = qGetR(orientationQInverse);
T = diag([1,1,1,1]);
T(1:3, 4) = -translation;

modelview = R * T;

% Output the transformed eye pose as optional 2nd return argument for reference:
eyePoseT = [translation, orientationQ];

return;
