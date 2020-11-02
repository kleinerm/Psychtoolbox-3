function glTexStorageMem3DEXT( target, levels, internalFormat, width, height, depth, memory, offset )

% glTexStorageMem3DEXT  Interface to OpenGL function glTexStorageMem3DEXT
%
% usage:  glTexStorageMem3DEXT( target, levels, internalFormat, width, height, depth, memory, offset )
%
% C function:  void glTexStorageMem3DEXT(GLenum target, GLsizei levels, GLenum internalFormat, GLsizei width, GLsizei height, GLsizei depth, GLuint memory, GLuint64 offset)

% 08-Aug-2020 -- created (generated automatically from header files)

if nargin~=8,
    error('invalid number of arguments');
end

moglcore( 'glTexStorageMem3DEXT', target, levels, internalFormat, width, height, depth, memory, uint64(offset) );

return
