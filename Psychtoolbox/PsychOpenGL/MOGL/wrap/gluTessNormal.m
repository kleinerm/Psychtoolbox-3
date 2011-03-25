function gluTessNormal( tess, valueX, valueY, valueZ )

% gluTessNormal  Interface to OpenGL function gluTessNormal
%
% usage:  gluTessNormal( tess, valueX, valueY, valueZ )
%
% C function:  void gluTessNormal(GLUtesselator* tess, GLdouble valueX, GLdouble valueY, GLdouble valueZ)

% 05-Mar-2006 -- created (generated automatically from header files)
% 24-Mar-2011 -- Made 64-bit clean (MK)

% ---allocate---
% ---protected---

if nargin~=4,
    error('invalid number of arguments');
end

if ~strcmp(class(tess),'double'),
	error([ 'argument ''tess'' must be a pointer coded as type double ' ]);
end

moglcore( 'gluTessNormal', tess, valueX, valueY, valueZ );

return
