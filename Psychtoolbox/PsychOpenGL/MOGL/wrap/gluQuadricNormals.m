function gluQuadricNormals( quad, normal )

% gluQuadricNormals  Interface to OpenGL function gluQuadricNormals
%
% usage:  gluQuadricNormals( quad, normal )
%
% C function:  void gluQuadricNormals(GLUquadric* quad, GLenum normal)

% 05-Mar-2006 -- created (generated automatically from header files)

if nargin~=2,
    error('invalid number of arguments');
end

if ~strcmp(class(quad),'uint32'),
	error([ 'argument ''quad'' must be a pointer coded as type uint32 ' ]);
end

moglcore( 'gluQuadricNormals', quad, normal );

return
