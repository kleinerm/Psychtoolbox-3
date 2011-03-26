function glLightiv( light, pname, params )

% glLightiv  Interface to OpenGL function glLightiv
%
% usage:  glLightiv( light, pname, params )
%
% C function:  void glLightiv(GLenum light, GLenum pname, const GLint* params)

% 25-Mar-2011 -- created (generated automatically from header files)

if nargin~=3,
    error('invalid number of arguments');
end

moglcore( 'glLightiv', light, pname, int32(params) );

return
