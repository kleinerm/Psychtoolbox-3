function gluTessVertex( tess , location, data )

% gluTessVertex  Interface to OpenGL function gluTessVertex
%
% usage:  gluTessVertex( tess, location, data )
%
% C function:  void gluTessVertex(GLUtesselator* tess, GLdouble* location, GLvoid* data)

% 05-Mar-2006 -- created (generated automatically from header files)
% 24-Mar-2011 -- Made 64-bit clean (MK)

% ---allocate---
% ---protected---

if nargin~=3,
    error('invalid number of arguments');
end

if ~strcmp(class(tess),'double'),
	error([ 'argument ''tess'' must be a pointer coded as type double ' ]);
end

moglcore( 'gluTessVertex', tess, location, data );

return
