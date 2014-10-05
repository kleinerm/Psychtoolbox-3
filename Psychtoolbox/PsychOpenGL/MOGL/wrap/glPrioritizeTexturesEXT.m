function glPrioritizeTexturesEXT( n, textures, priorities )

% glPrioritizeTexturesEXT  Interface to OpenGL function glPrioritizeTexturesEXT
%
% usage:  glPrioritizeTexturesEXT( n, textures, priorities )
%
% C function:  void glPrioritizeTexturesEXT(GLsizei n, const GLuint* textures, const GLclampf* priorities)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=3,
    error('invalid number of arguments');
end

moglcore( 'glPrioritizeTexturesEXT', n, uint32(textures), single(priorities) );

return
