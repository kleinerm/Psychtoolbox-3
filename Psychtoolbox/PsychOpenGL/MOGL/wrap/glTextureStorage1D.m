function glTextureStorage1D( texture, levels, internalformat, width )

% glTextureStorage1D  Interface to OpenGL function glTextureStorage1D
%
% usage:  glTextureStorage1D( texture, levels, internalformat, width )
%
% C function:  void glTextureStorage1D(GLuint texture, GLsizei levels, GLenum internalformat, GLsizei width)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=4,
    error('invalid number of arguments');
end

moglcore( 'glTextureStorage1D', texture, levels, internalformat, width );

return
