function glTextureStorage3D( texture, levels, internalformat, width, height, depth )

% glTextureStorage3D  Interface to OpenGL function glTextureStorage3D
%
% usage:  glTextureStorage3D( texture, levels, internalformat, width, height, depth )
%
% C function:  void glTextureStorage3D(GLuint texture, GLsizei levels, GLenum internalformat, GLsizei width, GLsizei height, GLsizei depth)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=6,
    error('invalid number of arguments');
end

moglcore( 'glTextureStorage3D', texture, levels, internalformat, width, height, depth );

return
