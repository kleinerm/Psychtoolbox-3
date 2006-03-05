function gluQuadricDrawStyle( quad, draw )

% gluQuadricDrawStyle  Interface to OpenGL function gluQuadricDrawStyle
%
% usage:  gluQuadricDrawStyle( quad, draw )
%
% C function:  void gluQuadricDrawStyle(GLUquadric* quad, GLenum draw)

% 05-Mar-2006 -- created (generated automatically from header files)

if nargin~=2,
    error('invalid number of arguments');
end

if ~strcmp(class(quad),'uint32'),
	error([ 'argument ''quad'' must be a pointer coded as type uint32 ' ]);
end

moglcore( 'gluQuadricDrawStyle', quad, draw );

return
