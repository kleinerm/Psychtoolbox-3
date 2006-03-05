function gluDeleteQuadric( quad )

% gluDeleteQuadric  Interface to OpenGL function gluDeleteQuadric
%
% usage:  gluDeleteQuadric( quad )
%
% C function:  void gluDeleteQuadric(GLUquadric* quad)

% 05-Mar-2006 -- created (generated automatically from header files)

if nargin~=1,
    error('invalid number of arguments');
end

if ~strcmp(class(quad),'uint32'),
	error([ 'argument ''quad'' must be a pointer coded as type uint32 ' ]);
end

moglcore( 'gluDeleteQuadric', quad );

return
