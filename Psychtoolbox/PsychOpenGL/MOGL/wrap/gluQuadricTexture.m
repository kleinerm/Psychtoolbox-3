function gluQuadricTexture( quad, texture )

% gluQuadricTexture  Interface to OpenGL function gluQuadricTexture
%
% usage:  gluQuadricTexture( quad, texture )
%
% C function:  void gluQuadricTexture(GLUquadric* quad, GLboolean texture)

% 05-Mar-2006 -- created (generated automatically from header files)

if nargin~=2,
    error('invalid number of arguments');
end

if ~strcmp(class(quad),'uint32'),
	error([ 'argument ''quad'' must be a pointer coded as type uint32 ' ]);
end

moglcore( 'gluQuadricTexture', quad, texture );

return
