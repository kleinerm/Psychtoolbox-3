function gluEndPolygon( tess )

% gluEndPolygon  Interface to OpenGL function gluEndPolygon
%
% usage:  gluEndPolygon( tess )
%
% C function:  void gluEndPolygon(GLUtesselator* tess)

% 05-Mar-2006 -- created (generated automatically from header files)

% ---protected---

error('gluEndPolygon is no longer supported! Use gluTessEndPolygon and gluTessEndContour instead.');

if nargin~=1,
    error('invalid number of arguments');
end

if ~strcmp(class(tess),'uint32'),
	error([ 'argument ''tess'' must be a pointer coded as type uint32 ' ]);
end

moglcore( 'gluEndPolygon', tess );

return
