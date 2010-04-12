function gluBeginPolygon( tess )

% gluBeginPolygon  Interface to OpenGL function gluBeginPolygon
%
% usage:  gluBeginPolygon( tess )
%
% C function:  void gluBeginPolygon(GLUtesselator* tess)

% 05-Mar-2006 -- created (generated automatically from header files)

% ---protected---

error('gluBeginPolygon is no longer supported! Use gluTessBeginPolygon and gluTessBeginContour instead.');

if nargin~=1,
    error('invalid number of arguments');
end

if ~strcmp(class(tess),'uint32'),
	error([ 'argument ''tess'' must be a pointer coded as type uint32 ' ]);
end

moglcore( 'gluBeginPolygon', tess );

return
