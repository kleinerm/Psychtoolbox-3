function gluTessBeginPolygon( tess, data )

% gluTessBeginPolygon  Interface to OpenGL function gluTessBeginPolygon
%
% usage:  gluTessBeginPolygon( tess, data )
%
% C function:  void gluTessBeginPolygon(GLUtesselator* tess, GLvoid* data)

% 05-Mar-2006 -- created (generated automatically from header files)

% ---allocate---
% ---protected---

if nargin~=2,
    error('invalid number of arguments');
end

if ~strcmp(class(tess),'uint32'),
	error([ 'argument ''tess'' must be a pointer coded as type uint32 ' ]);
end

moglcore( 'gluTessBeginPolygon', tess, data );

return
