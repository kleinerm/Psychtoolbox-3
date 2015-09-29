function M = PsychGetPositionYawMatrix(translation, yawRotation)
% M = PsychGetPositionYawMatrix(translation, yawRotation);
%
% Builds a 4x4 OpenGL compatible (right hand side post-multiply)
% matrix for a 3D translation followed by a rotation around the
% y-axis. Useful for positioning an observer in 3D space, then
% giving the observer a specific "looking direction" or heading.
%
% You can also use this function for incremental position and
% orientation updates by simply post-multiplying the result of
% this function to the result of a previous call to this function,
% e.g.:
%
% 1. Select start position and orientation:
%
%    M = PsychGetPositionYawMatrix(startPosition, startHeading);
%
% 2. Update location relative to current heading direction, then
%    change new heading by deltaHeading degrees for initiating a
%    turn:
%
%    M = M * PsychGetPositionYawMatrix(PositionIncrement, deltaHeading);
%
% Initializing an observer position with step 1, then updating it
% by repeating step 2, e.g., driven by keyboard input, mouse or
% gamepad input, or some other input device or "game logic" allows
% to move the observers body through the scene in a "natural" way.
%
%
% Parameters:
%
% 'translation' a 3 component translation vector [tx, ty, tz].
%
% 'yawRotation' a rotation angle in degrees along the y-axis, also
% known as heading direction or upright direction.
%

% History:
% 28-Sep-2015  mk  Written.

% Translation matrix:
T = diag([1 1 1 1]);
T(1:3, 4) = translation;

% Rotation matrix around y axis:
alpha = yawRotation / 180 * pi;
R = [cos(alpha)  0  sin(alpha) 0 ; ...
     0           1  0          0 ; ...
     -sin(alpha) 0  cos(alpha) 0 ; ...
     0           0  0          1];

% First translate, then rotate:
M = T * R;

end
