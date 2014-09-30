function pixels = glGetTextureImageEXT( texture, target, level, format, type )

% glGetTextureImageEXT  Interface to OpenGL function glGetTextureImageEXT
%
% usage:  pixels = glGetTextureImageEXT( texture, target, level, format, type )
%
% C function:  void glGetTextureImageEXT(GLuint texture, GLenum target, GLint level, GLenum format, GLenum type, void* pixels)

% 30-Sep-2014 -- created (generated automatically from header files)

% ---allocate---

if nargin~=5,
    error('invalid number of arguments');
end

pixels = (0);

moglcore( 'glGetTextureImageEXT', texture, target, level, format, type, pixels );

return
