function glTextureStorageMem3DEXT( texture, levels, internalFormat, width, height, depth, memory, offset )

% glTextureStorageMem3DEXT  Interface to OpenGL function glTextureStorageMem3DEXT
%
% usage:  glTextureStorageMem3DEXT( texture, levels, internalFormat, width, height, depth, memory, offset )
%
% C function:  void glTextureStorageMem3DEXT(GLuint texture, GLsizei levels, GLenum internalFormat, GLsizei width, GLsizei height, GLsizei depth, GLuint memory, GLuint64 offset)

% 08-Aug-2020 -- created (generated automatically from header files)

if nargin~=8,
    error('invalid number of arguments');
end

moglcore( 'glTextureStorageMem3DEXT', texture, levels, internalFormat, width, height, depth, memory, uint64(offset) );

return
