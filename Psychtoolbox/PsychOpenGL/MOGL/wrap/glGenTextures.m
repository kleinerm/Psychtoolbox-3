function textures = glGenTextures( n )

% glGenTextures  Interface to glGenTextures
% 
% usage:  textures = glGenTextures( n )
%
% C function:  void glGenTextures(GLsizei n, GLuint* textures)

% 20-Dec-2005 -- created (moglgen)

% ---allocate---
% ---protected---

textures = uint32(zeros(n+1,1));
moglcore( 'glGenTextures', n, textures );
textures = textures(1:end-1);
return
