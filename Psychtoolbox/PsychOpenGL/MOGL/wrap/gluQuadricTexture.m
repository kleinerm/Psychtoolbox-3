function gluQuadricTexture( quad, texture )

% gluQuadricTexture  Interface to OpenGL function gluQuadricTexture
%
% usage:  gluQuadricTexture( quad, texture )
%
% C function:  void gluQuadricTexture(GLUquadric* quad, GLboolean texture)

% 26-Mar-2011 -- created (generated automatically from header files)

if nargin~=2,
    error('invalid number of arguments');
end

if ~strcmp(class(quad),'double'),
	error([ 'argument ''quad'' must be a pointer coded as type double ' ]);
end

moglcore( 'gluQuadricTexture', quad, texture );

return
