function gluPartialDisk( quad, inner, outer, slices, loops, start, sweep )

% gluPartialDisk  Interface to OpenGL function gluPartialDisk
%
% usage:  gluPartialDisk( quad, inner, outer, slices, loops, start, sweep )
%
% C function:  void gluPartialDisk(GLUquadric* quad, GLdouble inner, GLdouble outer, GLint slices, GLint loops, GLdouble start, GLdouble sweep)

% 05-Mar-2006 -- created (generated automatically from header files)

if nargin~=7,
    error('invalid number of arguments');
end

if ~strcmp(class(quad),'uint32'),
	error([ 'argument ''quad'' must be a pointer coded as type uint32 ' ]);
end

moglcore( 'gluPartialDisk', quad, inner, outer, slices, loops, start, sweep );

return
