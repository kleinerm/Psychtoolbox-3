function textures = glGenTextures( n )

% glGenTextures  Interface to glGenTextures
% 
% usage:  textures = glGenTextures( n )
%
% C function:  void glGenTextures(GLsizei n, GLuint* textures)

% 20-Dec-2005 -- created (moglgen)

% ---allocate---
% ---protected---

textures = uint32(zeros(n,1));
moglcore( 'glGenTextures', n, textures );
textures = double(textures);

return


% ---autocode---
%
% function textures = glGenTextures( n )
% 
% % glGenTextures  Interface to OpenGL function glGenTextures
% %
% % usage:  textures = glGenTextures( n )
% %
% % C function:  void glGenTextures(GLsizei n, GLuint* textures)
% 
% % 05-Mar-2006 -- created (generated automatically from header files)
% 
% % ---allocate---
% 
% if nargin~=1,
%     error('invalid number of arguments');
% end
% 
% textures = uint32(0);
% 
% moglcore( 'glGenTextures', n, textures );
% 
% return
%
