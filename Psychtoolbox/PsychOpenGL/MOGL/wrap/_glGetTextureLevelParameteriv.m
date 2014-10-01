function params = glGetTextureLevelParameteriv( texture, level, pname )

% glGetTextureLevelParameteriv  Interface to OpenGL function glGetTextureLevelParameteriv
%
% usage:  params = glGetTextureLevelParameteriv( texture, level, pname )
%
% C function:  void glGetTextureLevelParameteriv(GLuint texture, GLint level, GLenum pname, GLint* params)

% 30-Sep-2014 -- created (generated automatically from header files)

% ---allocate---

if nargin~=3,
    error('invalid number of arguments');
end

params = int32(0);

moglcore( 'glGetTextureLevelParameteriv', texture, level, pname, params );

return
