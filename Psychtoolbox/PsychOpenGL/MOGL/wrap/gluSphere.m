function gluSphere( quad, radius, slices, stacks )

% gluSphere  Interface to OpenGL function gluSphere
%
% usage:  gluSphere( quad, radius, slices, stacks )
%
% C function:  void gluSphere(GLUquadric* quad, GLdouble radius, GLint slices, GLint stacks)

% 26-Mar-2011 -- created (generated automatically from header files)

if nargin~=4,
    error('invalid number of arguments');
end

if ~strcmp(class(quad),'double'),
	error([ 'argument ''quad'' must be a pointer coded as type double ' ]);
end

moglcore( 'gluSphere', quad, radius, slices, stacks );

return
