function glPrioritizeTextures( n, textures, priorities )

% glPrioritizeTextures  Interface to OpenGL function glPrioritizeTextures
%
% usage:  glPrioritizeTextures( n, textures, priorities )
%
% C function:  void glPrioritizeTextures(GLsizei n, const GLuint* textures, const GLclampf* priorities)

% 05-Mar-2006 -- created (generated automatically from header files)

if nargin~=3,
    error('invalid number of arguments');
end

moglcore( 'glPrioritizeTextures', n, uint32(textures), moglsingle(priorities) );

return
