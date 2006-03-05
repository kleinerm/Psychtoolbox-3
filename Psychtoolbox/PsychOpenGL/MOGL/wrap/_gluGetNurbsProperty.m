function data = gluGetNurbsProperty( nurb, property )

% gluGetNurbsProperty  Interface to OpenGL function gluGetNurbsProperty
%
% usage:  data = gluGetNurbsProperty( nurb, property )
%
% C function:  void gluGetNurbsProperty(GLUnurbs* nurb, GLenum property, GLfloat* data)

% 05-Mar-2006 -- created (generated automatically from header files)

% ---allocate---

if nargin~=2,
    error('invalid number of arguments');
end

if ~strcmp(class(nurb),'uint32'),
	error([ 'argument ''nurb'' must be a pointer coded as type uint32 ' ]);
end

data = single(0);

moglcore( 'gluGetNurbsProperty', nurb, property, data );

return
