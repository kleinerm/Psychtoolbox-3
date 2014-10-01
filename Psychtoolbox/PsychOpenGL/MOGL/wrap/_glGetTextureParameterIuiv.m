function params = glGetTextureParameterIuiv( texture, pname )

% glGetTextureParameterIuiv  Interface to OpenGL function glGetTextureParameterIuiv
%
% usage:  params = glGetTextureParameterIuiv( texture, pname )
%
% C function:  void glGetTextureParameterIuiv(GLuint texture, GLenum pname, GLuint* params)

% 30-Sep-2014 -- created (generated automatically from header files)

% ---allocate---

if nargin~=2,
    error('invalid number of arguments');
end

params = uint32(0);

moglcore( 'glGetTextureParameterIuiv', texture, pname, params );

return
