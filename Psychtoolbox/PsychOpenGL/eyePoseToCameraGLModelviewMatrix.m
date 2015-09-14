function modelview = eyePoseToCameraGLModelviewMatrix(eyePose)
% eyePoseToCameraGLModelviewMatrix() - Convert eyePose directly to OpenGL modelview matrix.
%
% Usage:
%
% modelview = eyePoseToCameraGLModelviewMatrix(eyePose)
%
% 'eyePose' is a [tx, ty, tz, rx, ry, rz, rw] vector, with the first 3 components
% defining eye translation, and the last 4 components defining a rotation Quaternion
% that defines eye orientation.
%
% modelview is a 4x4 matrix that can be directly loaded into OpenGL as GL_MODELVIEW
% matrix, e.g., glLoadMatrixd(modelview); to define camera position and orientation
% for rendering accordign to eyePose. Iow. modelview is already the inverse matrix of
% eyePose.
%

% History:
% 14-Sep-2015  mk  Written.

translation = eyePose(1:3);
orientationQ = eyePose(4:7);
orientationQInverse = [-orientationQ(1:3), orientationQ(4)];
R = diag([1,1,1,1]);
R(1:3, 1:3) = qGetR(orientationQInverse);
T = diag([1,1,1,1]);
T(1:3, 4) = -translation;

modelview = R * T;

return;
