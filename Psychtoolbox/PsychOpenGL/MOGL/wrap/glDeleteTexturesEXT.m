function glDeleteTexturesEXT( n, textures )

% glDeleteTexturesEXT  Interface to OpenGL function glDeleteTexturesEXT
%
% usage:  glDeleteTexturesEXT( n, textures )
%
% C function:  void glDeleteTexturesEXT(GLsizei n, const GLuint* textures)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=2,
    error('invalid number of arguments');
end

moglcore( 'glDeleteTexturesEXT', n, uint32(textures) );

return
