function params = glGetTextureLevelParameterivEXT( texture, target, level, pname )

% glGetTextureLevelParameterivEXT  Interface to OpenGL function glGetTextureLevelParameterivEXT
%
% usage:  params = glGetTextureLevelParameterivEXT( texture, target, level, pname )
%
% C function:  void glGetTextureLevelParameterivEXT(GLuint texture, GLenum target, GLint level, GLenum pname, GLint* params)

% 30-Sep-2014 -- created (generated automatically from header files)

% ---allocate---

if nargin~=4,
    error('invalid number of arguments');
end

params = int32(0);

moglcore( 'glGetTextureLevelParameterivEXT', texture, target, level, pname, params );

return
