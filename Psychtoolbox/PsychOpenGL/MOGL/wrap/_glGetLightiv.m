function params = glGetLightiv( light, pname )

% glGetLightiv  Interface to OpenGL function glGetLightiv
%
% usage:  params = glGetLightiv( light, pname )
%
% C function:  void glGetLightiv(GLenum light, GLenum pname, GLint* params)

% 05-Mar-2006 -- created (generated automatically from header files)

% ---allocate---

if nargin~=2,
    error('invalid number of arguments');
end

params = int32(0);

moglcore( 'glGetLightiv', light, pname, params );

return
