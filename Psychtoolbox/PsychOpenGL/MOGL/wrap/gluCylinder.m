function gluCylinder( quad, base, top, height, slices, stacks )

% gluCylinder  Interface to OpenGL function gluCylinder
%
% usage:  gluCylinder( quad, base, top, height, slices, stacks )
%
% C function:  void gluCylinder(GLUquadric* quad, GLdouble base, GLdouble top, GLdouble height, GLint slices, GLint stacks)

% 05-Mar-2006 -- created (generated automatically from header files)

if nargin~=6,
    error('invalid number of arguments');
end

if ~strcmp(class(quad),'uint32'),
	error([ 'argument ''quad'' must be a pointer coded as type uint32 ' ]);
end

moglcore( 'gluCylinder', quad, base, top, height, slices, stacks );

return
