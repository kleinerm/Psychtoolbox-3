function img = glGetCompressedTextureImageEXT( texture, target, lod )

% glGetCompressedTextureImageEXT  Interface to OpenGL function glGetCompressedTextureImageEXT
%
% usage:  img = glGetCompressedTextureImageEXT( texture, target, lod )
%
% C function:  void glGetCompressedTextureImageEXT(GLuint texture, GLenum target, GLint lod, void* img)

% 30-Sep-2014 -- created (generated automatically from header files)

% ---allocate---

if nargin~=3,
    error('invalid number of arguments');
end

img = (0);

moglcore( 'glGetCompressedTextureImageEXT', texture, target, lod, img );

return
