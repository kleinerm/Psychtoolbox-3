function glTextureStorageSparseAMD( texture, target, internalFormat, width, height, depth, layers, flags )

% glTextureStorageSparseAMD  Interface to OpenGL function glTextureStorageSparseAMD
%
% usage:  glTextureStorageSparseAMD( texture, target, internalFormat, width, height, depth, layers, flags )
%
% C function:  void glTextureStorageSparseAMD(GLuint texture, GLenum target, GLenum internalFormat, GLsizei width, GLsizei height, GLsizei depth, GLsizei layers, GLbitfield flags)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=8,
    error('invalid number of arguments');
end

moglcore( 'glTextureStorageSparseAMD', texture, target, internalFormat, width, height, depth, layers, flags );

return
