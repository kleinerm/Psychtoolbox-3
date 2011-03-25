function gluTessBeginPolygon( tess, data )

% gluTessBeginPolygon  Interface to OpenGL function gluTessBeginPolygon
%
% usage:  gluTessBeginPolygon( tess, data )
%
% C function:  void gluTessBeginPolygon(GLUtesselator* tess, GLvoid* data)

% 05-Mar-2006 -- created (generated automatically from header files)
% 24-Mar-2011 -- Made 64-bit clean (MK)

% ---allocate---
% ---protected---

if nargin~=2,
    error('invalid number of arguments');
end

if ~strcmp(class(tess),'double'),
	error([ 'argument ''tess'' must be a pointer coded as type double ' ]);
end

moglcore( 'gluTessBeginPolygon', tess, data );

return
