function params = glGetTextureLevelParameterfv( texture, level, pname )

% glGetTextureLevelParameterfv  Interface to OpenGL function glGetTextureLevelParameterfv
%
% usage:  params = glGetTextureLevelParameterfv( texture, level, pname )
%
% C function:  void glGetTextureLevelParameterfv(GLuint texture, GLint level, GLenum pname, GLfloat* params)

% 30-Sep-2014 -- created (generated automatically from header files)

% ---allocate---

if nargin~=3,
    error('invalid number of arguments');
end

params = single(0);

moglcore( 'glGetTextureLevelParameterfv', texture, level, pname, params );

return
