function data = gluTessBeginPolygon( tess )

% gluTessBeginPolygon  Interface to OpenGL function gluTessBeginPolygon
%
% usage:  data = gluTessBeginPolygon( tess )
%
% C function:  void gluTessBeginPolygon(GLUtesselator* tess, GLvoid* data)

% 05-Mar-2006 -- created (generated automatically from header files)

% ---allocate---

if nargin~=1,
    error('invalid number of arguments');
end

if ~strcmp(class(tess),'uint32'),
	error([ 'argument ''tess'' must be a pointer coded as type uint32 ' ]);
end

data = (0);

moglcore( 'gluTessBeginPolygon', tess, data );

return
