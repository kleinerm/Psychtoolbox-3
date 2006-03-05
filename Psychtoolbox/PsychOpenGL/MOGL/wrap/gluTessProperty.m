function gluTessProperty( tess, which, data )

% gluTessProperty  Interface to OpenGL function gluTessProperty
%
% usage:  gluTessProperty( tess, which, data )
%
% C function:  void gluTessProperty(GLUtesselator* tess, GLenum which, GLdouble data)

% 05-Mar-2006 -- created (generated automatically from header files)

if nargin~=3,
    error('invalid number of arguments');
end

if ~strcmp(class(tess),'uint32'),
	error([ 'argument ''tess'' must be a pointer coded as type uint32 ' ]);
end

moglcore( 'gluTessProperty', tess, which, data );

return
