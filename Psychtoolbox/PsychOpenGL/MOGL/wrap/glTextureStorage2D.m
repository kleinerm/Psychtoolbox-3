function glTextureStorage2D( texture, levels, internalformat, width, height )

% glTextureStorage2D  Interface to OpenGL function glTextureStorage2D
%
% usage:  glTextureStorage2D( texture, levels, internalformat, width, height )
%
% C function:  void glTextureStorage2D(GLuint texture, GLsizei levels, GLenum internalformat, GLsizei width, GLsizei height)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=5,
    error('invalid number of arguments');
end

moglcore( 'glTextureStorage2D', texture, levels, internalformat, width, height );

return
