function gluDisk( quad, inner, outer, slices, loops )

% gluDisk  Interface to OpenGL function gluDisk
%
% usage:  gluDisk( quad, inner, outer, slices, loops )
%
% C function:  void gluDisk(GLUquadric* quad, GLdouble inner, GLdouble outer, GLint slices, GLint loops)

% 25-Mar-2011 -- created (generated automatically from header files)

if nargin~=5,
    error('invalid number of arguments');
end

if ~strcmp(class(quad),'double'),
	error([ 'argument ''quad'' must be a pointer coded as type double ' ]);
end

moglcore( 'gluDisk', quad, inner, outer, slices, loops );

return
