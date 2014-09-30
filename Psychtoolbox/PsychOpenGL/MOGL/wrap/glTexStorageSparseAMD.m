function glTexStorageSparseAMD( target, internalFormat, width, height, depth, layers, flags )

% glTexStorageSparseAMD  Interface to OpenGL function glTexStorageSparseAMD
%
% usage:  glTexStorageSparseAMD( target, internalFormat, width, height, depth, layers, flags )
%
% C function:  void glTexStorageSparseAMD(GLenum target, GLenum internalFormat, GLsizei width, GLsizei height, GLsizei depth, GLsizei layers, GLbitfield flags)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=7,
    error('invalid number of arguments');
end

moglcore( 'glTexStorageSparseAMD', target, internalFormat, width, height, depth, layers, flags );

return
