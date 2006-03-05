function gluQuadricOrientation( quad, orientation )

% gluQuadricOrientation  Interface to OpenGL function gluQuadricOrientation
%
% usage:  gluQuadricOrientation( quad, orientation )
%
% C function:  void gluQuadricOrientation(GLUquadric* quad, GLenum orientation)

% 05-Mar-2006 -- created (generated automatically from header files)

if nargin~=2,
    error('invalid number of arguments');
end

if ~strcmp(class(quad),'uint32'),
	error([ 'argument ''quad'' must be a pointer coded as type uint32 ' ]);
end

moglcore( 'gluQuadricOrientation', quad, orientation );

return
