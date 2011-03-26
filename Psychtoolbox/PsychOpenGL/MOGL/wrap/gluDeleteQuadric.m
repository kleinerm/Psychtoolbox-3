function gluDeleteQuadric( quad )

% gluDeleteQuadric  Interface to OpenGL function gluDeleteQuadric
%
% usage:  gluDeleteQuadric( quad )
%
% C function:  void gluDeleteQuadric(GLUquadric* quad)

% 25-Mar-2011 -- created (generated automatically from header files)

if nargin~=1,
    error('invalid number of arguments');
end

if ~strcmp(class(quad),'double'),
	error([ 'argument ''quad'' must be a pointer coded as type double ' ]);
end

moglcore( 'gluDeleteQuadric', quad );

return
