function gluTessBeginContour( tess )

% gluTessBeginContour  Interface to OpenGL function gluTessBeginContour
%
% usage:  gluTessBeginContour( tess )
%
% C function:  void gluTessBeginContour(GLUtesselator* tess)

% 05-Mar-2006 -- created (generated automatically from header files)
% 24-Mar-2011 -- Made 64-bit clean (MK)

% ---allocate---
% ---protected---

if nargin~=1,
    error('invalid number of arguments');
end

if ~strcmp(class(tess),'double'),
	error([ 'argument ''tess'' must be a pointer coded as type double ' ]);
end

moglcore( 'gluTessBeginContour', tess );

return
