function params = glGetTextureParameteriv( texture, pname )

% glGetTextureParameteriv  Interface to OpenGL function glGetTextureParameteriv
%
% usage:  params = glGetTextureParameteriv( texture, pname )
%
% C function:  void glGetTextureParameteriv(GLuint texture, GLenum pname, GLint* params)

% 30-Sep-2014 -- created (generated automatically from header files)

% ---allocate---

if nargin~=2,
    error('invalid number of arguments');
end

params = int32(0);

moglcore( 'glGetTextureParameteriv', texture, pname, params );

return
