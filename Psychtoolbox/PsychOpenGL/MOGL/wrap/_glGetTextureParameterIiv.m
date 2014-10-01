function params = glGetTextureParameterIiv( texture, pname )

% glGetTextureParameterIiv  Interface to OpenGL function glGetTextureParameterIiv
%
% usage:  params = glGetTextureParameterIiv( texture, pname )
%
% C function:  void glGetTextureParameterIiv(GLuint texture, GLenum pname, GLint* params)

% 30-Sep-2014 -- created (generated automatically from header files)

% ---allocate---

if nargin~=2,
    error('invalid number of arguments');
end

params = int32(0);

moglcore( 'glGetTextureParameterIiv', texture, pname, params );

return
