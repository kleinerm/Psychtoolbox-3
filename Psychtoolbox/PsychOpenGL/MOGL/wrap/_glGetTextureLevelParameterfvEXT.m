function params = glGetTextureLevelParameterfvEXT( texture, target, level, pname )

% glGetTextureLevelParameterfvEXT  Interface to OpenGL function glGetTextureLevelParameterfvEXT
%
% usage:  params = glGetTextureLevelParameterfvEXT( texture, target, level, pname )
%
% C function:  void glGetTextureLevelParameterfvEXT(GLuint texture, GLenum target, GLint level, GLenum pname, GLfloat* params)

% 30-Sep-2014 -- created (generated automatically from header files)

% ---allocate---

if nargin~=4,
    error('invalid number of arguments');
end

params = single(0);

moglcore( 'glGetTextureLevelParameterfvEXT', texture, target, level, pname, params );

return
