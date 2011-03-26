function data = gluGetTessProperty( tess, which )

% gluGetTessProperty  Interface to OpenGL function gluGetTessProperty
%
% usage:  data = gluGetTessProperty( tess, which )
%
% C function:  void gluGetTessProperty(GLUtesselator* tess, GLenum which, GLdouble* data)

% 24-Jan-2006 -- created (generated automatically from header files)

% ---allocate---
% ---protected---
% ---skip---

if nargin~=2,
    error('invalid number of arguments');
end

if ~strcmp(class(tess),'double'),
	error([ 'argument ''tess'' must be a pointer coded as type double ' ]);
end

data = double(0);
moglcore( 'gluGetTessProperty', tess, which, data );

return
