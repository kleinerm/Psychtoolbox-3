function gluNurbsProperty( nurb, property, value )

% gluNurbsProperty  Interface to OpenGL function gluNurbsProperty
%
% usage:  gluNurbsProperty( nurb, property, value )
%
% C function:  void gluNurbsProperty(GLUnurbs* nurb, GLenum property, GLfloat value)

% 26-Mar-2011 -- created (generated automatically from header files)

if nargin~=3,
    error('invalid number of arguments');
end

if ~strcmp(class(nurb),'double'),
	error([ 'argument ''nurb'' must be a pointer coded as type double ' ]);
end

moglcore( 'gluNurbsProperty', nurb, property, value );

return
