function glTexStorageMem1DEXT( target, levels, internalFormat, width, memory, offset )

% glTexStorageMem1DEXT  Interface to OpenGL function glTexStorageMem1DEXT
%
% usage:  glTexStorageMem1DEXT( target, levels, internalFormat, width, memory, offset )
%
% C function:  void glTexStorageMem1DEXT(GLenum target, GLsizei levels, GLenum internalFormat, GLsizei width, GLuint memory, GLuint64 offset)

% 08-Aug-2020 -- created (generated automatically from header files)

if nargin~=6,
    error('invalid number of arguments');
end

moglcore( 'glTexStorageMem1DEXT', target, levels, internalFormat, width, memory, uint64(offset) );

return
