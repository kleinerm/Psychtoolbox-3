function gluTessVertex( tess , location, data )

% gluTessVertex  Interface to OpenGL function gluTessVertex
%
% usage:  gluTessVertex( tess, location, data )
%
% C function:  void gluTessVertex(GLUtesselator* tess, GLdouble* location, GLvoid* data)

% 05-Mar-2006 -- created (generated automatically from header files)

% ---allocate---
% ---protected---

if nargin~=3,
    error('invalid number of arguments');
end

if ~strcmp(class(tess),'uint32'),
	error([ 'argument ''tess'' must be a pointer coded as type uint32 ' ]);
end

moglcore( 'gluTessVertex', tess, location, data );

return
