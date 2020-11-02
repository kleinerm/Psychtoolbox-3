function glTextureStorageMem1DEXT( texture, levels, internalFormat, width, memory, offset )

% glTextureStorageMem1DEXT  Interface to OpenGL function glTextureStorageMem1DEXT
%
% usage:  glTextureStorageMem1DEXT( texture, levels, internalFormat, width, memory, offset )
%
% C function:  void glTextureStorageMem1DEXT(GLuint texture, GLsizei levels, GLenum internalFormat, GLsizei width, GLuint memory, GLuint64 offset)

% 08-Aug-2020 -- created (generated automatically from header files)

if nargin~=6,
    error('invalid number of arguments');
end

moglcore( 'glTextureStorageMem1DEXT', texture, levels, internalFormat, width, memory, uint64(offset) );

return
