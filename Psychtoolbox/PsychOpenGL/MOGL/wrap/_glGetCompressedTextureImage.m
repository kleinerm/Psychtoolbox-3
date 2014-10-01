function pixels = glGetCompressedTextureImage( texture, level, bufSize )

% glGetCompressedTextureImage  Interface to OpenGL function glGetCompressedTextureImage
%
% usage:  pixels = glGetCompressedTextureImage( texture, level, bufSize )
%
% C function:  void glGetCompressedTextureImage(GLuint texture, GLint level, GLsizei bufSize, void* pixels)

% 30-Sep-2014 -- created (generated automatically from header files)

% ---allocate---

if nargin~=3,
    error('invalid number of arguments');
end

pixels = (0);

moglcore( 'glGetCompressedTextureImage', texture, level, bufSize, pixels );

return
