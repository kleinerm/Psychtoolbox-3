function gluTessEndPolygon( tess )

% gluTessEndPolygon  Interface to OpenGL function gluTessEndPolygon
%
% usage:  gluTessEndPolygon( tess )
%
% C function:  void gluTessEndPolygon(GLUtesselator* tess)

% 05-Mar-2006 -- created (generated automatically from header files)

if nargin~=1,
    error('invalid number of arguments');
end

if ~strcmp(class(tess),'uint32'),
	error([ 'argument ''tess'' must be a pointer coded as type uint32 ' ]);
end

moglcore( 'gluTessEndPolygon', tess );

return
