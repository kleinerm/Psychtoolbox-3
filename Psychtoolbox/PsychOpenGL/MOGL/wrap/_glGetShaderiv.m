function params = glGetShaderiv( shader, pname )

% glGetShaderiv  Interface to OpenGL function glGetShaderiv
%
% usage:  params = glGetShaderiv( shader, pname )
%
% C function:  void glGetShaderiv(GLuint shader, GLenum pname, GLint* params)

% 05-Mar-2006 -- created (generated automatically from header files)

% ---allocate---

if nargin~=2,
    error('invalid number of arguments');
end

params = int32(0);

moglcore( 'glGetShaderiv', shader, pname, params );

return
