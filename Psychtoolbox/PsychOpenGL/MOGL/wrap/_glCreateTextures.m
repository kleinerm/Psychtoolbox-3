function textures = glCreateTextures( target, n )

% glCreateTextures  Interface to OpenGL function glCreateTextures
%
% usage:  textures = glCreateTextures( target, n )
%
% C function:  void glCreateTextures(GLenum target, GLsizei n, GLuint* textures)

% 30-Sep-2014 -- created (generated automatically from header files)

% ---allocate---

if nargin~=2,
    error('invalid number of arguments');
end

textures = uint32(0);

moglcore( 'glCreateTextures', target, n, textures );

return
