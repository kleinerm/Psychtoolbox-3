function glBindImageTextures( first, count, textures )

% glBindImageTextures  Interface to OpenGL function glBindImageTextures
%
% usage:  glBindImageTextures( first, count, textures )
%
% C function:  void glBindImageTextures(GLuint first, GLsizei count, const GLuint* textures)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=3,
    error('invalid number of arguments');
end

moglcore( 'glBindImageTextures', first, count, uint32(textures) );

return
