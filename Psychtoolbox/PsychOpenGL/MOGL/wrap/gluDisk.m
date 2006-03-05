function gluDisk( quad, inner, outer, slices, loops )

% gluDisk  Interface to OpenGL function gluDisk
%
% usage:  gluDisk( quad, inner, outer, slices, loops )
%
% C function:  void gluDisk(GLUquadric* quad, GLdouble inner, GLdouble outer, GLint slices, GLint loops)

% 05-Mar-2006 -- created (generated automatically from header files)

if nargin~=5,
    error('invalid number of arguments');
end

if ~strcmp(class(quad),'uint32'),
	error([ 'argument ''quad'' must be a pointer coded as type uint32 ' ]);
end

moglcore( 'gluDisk', quad, inner, outer, slices, loops );

return
