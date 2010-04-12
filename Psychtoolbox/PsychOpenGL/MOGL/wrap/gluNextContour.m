function gluNextContour( tess, type )

% gluNextContour  Interface to OpenGL function gluNextContour
%
% usage:  gluNextContour( tess, type )
%
% C function:  void gluNextContour(GLUtesselator* tess, GLenum type)

% 05-Mar-2006 -- created (generated automatically from header files)

% ---protected---

error('gluNextContour is no longer supported! Use gluTessEndContour and gluTessBeginContour instead.');

if nargin~=2,
    error('invalid number of arguments');
end

if ~strcmp(class(tess),'uint32'),
	error([ 'argument ''tess'' must be a pointer coded as type uint32 ' ]);
end

moglcore( 'gluNextContour', tess, type );

return
