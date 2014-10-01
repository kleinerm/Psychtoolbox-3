function glBindTextures( first, count, textures )

% glBindTextures  Interface to OpenGL function glBindTextures
%
% usage:  glBindTextures( first, count, textures )
%
% C function:  void glBindTextures(GLuint first, GLsizei count, const GLuint* textures)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=3,
    error('invalid number of arguments');
end

moglcore( 'glBindTextures', first, count, uint32(textures) );

return
