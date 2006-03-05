function [ location, data ] = gluTessVertex( tess )

% gluTessVertex  Interface to OpenGL function gluTessVertex
%
% usage:  [ location, data ] = gluTessVertex( tess )
%
% C function:  void gluTessVertex(GLUtesselator* tess, GLdouble* location, GLvoid* data)

% 05-Mar-2006 -- created (generated automatically from header files)

% ---allocate---

if nargin~=1,
    error('invalid number of arguments');
end

if ~strcmp(class(tess),'uint32'),
	error([ 'argument ''tess'' must be a pointer coded as type uint32 ' ]);
end

location = double(0);
data = (0);

moglcore( 'gluTessVertex', tess, location, data );

return
