function gluQuadricOrientation( quad, orientation )

% gluQuadricOrientation  Interface to OpenGL function gluQuadricOrientation
%
% usage:  gluQuadricOrientation( quad, orientation )
%
% C function:  void gluQuadricOrientation(GLUquadric* quad, GLenum orientation)

% 26-Mar-2011 -- created (generated automatically from header files)

if nargin~=2,
    error('invalid number of arguments');
end

if ~strcmp(class(quad),'double'),
	error([ 'argument ''quad'' must be a pointer coded as type double ' ]);
end

moglcore( 'gluQuadricOrientation', quad, orientation );

return
