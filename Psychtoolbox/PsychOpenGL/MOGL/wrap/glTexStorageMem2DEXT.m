function glTexStorageMem2DEXT( target, levels, internalFormat, width, height, memory, offset )

% glTexStorageMem2DEXT  Interface to OpenGL function glTexStorageMem2DEXT
%
% usage:  glTexStorageMem2DEXT( target, levels, internalFormat, width, height, memory, offset )
%
% C function:  void glTexStorageMem2DEXT(GLenum target, GLsizei levels, GLenum internalFormat, GLsizei width, GLsizei height, GLuint memory, GLuint64 offset)

% 08-Aug-2020 -- created (generated automatically from header files)

if nargin~=7,
    error('invalid number of arguments');
end

moglcore( 'glTexStorageMem2DEXT', target, levels, internalFormat, width, height, memory, uint64(offset) );

return
