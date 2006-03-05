function glDeleteTextures( n, textures )

% glDeleteTextures  Interface to OpenGL function glDeleteTextures
%
% usage:  glDeleteTextures( n, textures )
%
% C function:  void glDeleteTextures(GLsizei n, const GLuint* textures)

% 05-Mar-2006 -- created (generated automatically from header files)

if nargin~=2,
    error('invalid number of arguments');
end

moglcore( 'glDeleteTextures', n, uint32(textures) );

return
