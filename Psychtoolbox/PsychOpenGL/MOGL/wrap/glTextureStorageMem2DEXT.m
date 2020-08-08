function glTextureStorageMem2DEXT( texture, levels, internalFormat, width, height, memory, offset )

% glTextureStorageMem2DEXT  Interface to OpenGL function glTextureStorageMem2DEXT
%
% usage:  glTextureStorageMem2DEXT( texture, levels, internalFormat, width, height, memory, offset )
%
% C function:  void glTextureStorageMem2DEXT(GLuint texture, GLsizei levels, GLenum internalFormat, GLsizei width, GLsizei height, GLuint memory, GLuint64 offset)

% 08-Aug-2020 -- created (generated automatically from header files)

if nargin~=7,
    error('invalid number of arguments');
end

moglcore( 'glTextureStorageMem2DEXT', texture, levels, internalFormat, width, height, memory, uint64(offset) );

return
