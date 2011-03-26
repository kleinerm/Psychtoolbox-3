function gluQuadricDrawStyle( quad, draw )

% gluQuadricDrawStyle  Interface to OpenGL function gluQuadricDrawStyle
%
% usage:  gluQuadricDrawStyle( quad, draw )
%
% C function:  void gluQuadricDrawStyle(GLUquadric* quad, GLenum draw)

% 26-Mar-2011 -- created (generated automatically from header files)

if nargin~=2,
    error('invalid number of arguments');
end

if ~strcmp(class(quad),'double'),
	error([ 'argument ''quad'' must be a pointer coded as type double ' ]);
end

moglcore( 'gluQuadricDrawStyle', quad, draw );

return
