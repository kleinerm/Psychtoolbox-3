function gluTessEndContour( tess )

% gluTessEndContour  Interface to OpenGL function gluTessEndContour
%
% usage:  gluTessEndContour( tess )
%
% C function:  void gluTessEndContour(GLUtesselator* tess)

% 05-Mar-2006 -- created (generated automatically from header files)

if nargin~=1,
    error('invalid number of arguments');
end

if ~strcmp(class(tess),'uint32'),
	error([ 'argument ''tess'' must be a pointer coded as type uint32 ' ]);
end

moglcore( 'gluTessEndContour', tess );

return
