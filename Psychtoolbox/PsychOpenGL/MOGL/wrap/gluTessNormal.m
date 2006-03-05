function gluTessNormal( tess, valueX, valueY, valueZ )

% gluTessNormal  Interface to OpenGL function gluTessNormal
%
% usage:  gluTessNormal( tess, valueX, valueY, valueZ )
%
% C function:  void gluTessNormal(GLUtesselator* tess, GLdouble valueX, GLdouble valueY, GLdouble valueZ)

% 05-Mar-2006 -- created (generated automatically from header files)

if nargin~=4,
    error('invalid number of arguments');
end

if ~strcmp(class(tess),'uint32'),
	error([ 'argument ''tess'' must be a pointer coded as type uint32 ' ]);
end

moglcore( 'gluTessNormal', tess, valueX, valueY, valueZ );

return
