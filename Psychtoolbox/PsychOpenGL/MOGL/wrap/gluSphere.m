function gluSphere( quad, radius, slices, stacks )

% gluSphere  Interface to OpenGL function gluSphere
%
% usage:  gluSphere( quad, radius, slices, stacks )
%
% C function:  void gluSphere(GLUquadric* quad, GLdouble radius, GLint slices, GLint stacks)

% 05-Mar-2006 -- created (generated automatically from header files)

if nargin~=4,
    error('invalid number of arguments');
end

if ~strcmp(class(quad),'uint32'),
	error([ 'argument ''quad'' must be a pointer coded as type uint32 ' ]);
end

moglcore( 'gluSphere', quad, radius, slices, stacks );

return
