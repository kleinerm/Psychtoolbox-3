function textures = glGenTexturesEXT( n )

% glGenTexturesEXT  Interface to OpenGL function glGenTexturesEXT
%
% usage:  textures = glGenTexturesEXT( n )
%
% C function:  void glGenTexturesEXT(GLsizei n, GLuint* textures)

% 30-Sep-2014 -- created (generated automatically from header files)

% ---allocate---

if nargin~=1,
    error('invalid number of arguments');
end

textures = uint32(0);

moglcore( 'glGenTexturesEXT', n, textures );

return
