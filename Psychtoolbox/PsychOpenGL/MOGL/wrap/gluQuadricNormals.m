function gluQuadricNormals( quad, normal )

% gluQuadricNormals  Interface to OpenGL function gluQuadricNormals
%
% usage:  gluQuadricNormals( quad, normal )
%
% C function:  void gluQuadricNormals(GLUquadric* quad, GLenum normal)

% 26-Mar-2011 -- created (generated automatically from header files)

if nargin~=2,
    error('invalid number of arguments');
end

if ~strcmp(class(quad),'double'),
	error([ 'argument ''quad'' must be a pointer coded as type double ' ]);
end

moglcore( 'gluQuadricNormals', quad, normal );

return
