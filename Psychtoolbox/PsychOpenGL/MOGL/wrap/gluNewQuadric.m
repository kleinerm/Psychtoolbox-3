function r = gluNewQuadric

% gluNewQuadric  Interface to OpenGL function gluNewQuadric
%
% usage:  r = gluNewQuadric
%
% C function:  GLUquadric* gluNewQuadric(void)

% 25-Mar-2011 -- created (generated automatically from header files)

if nargin~=0,
    error('invalid number of arguments');
end

r = moglcore( 'gluNewQuadric' );

return
