function glBindTextureEXT( target, texture )

% glBindTextureEXT  Interface to OpenGL function glBindTextureEXT
%
% usage:  glBindTextureEXT( target, texture )
%
% C function:  void glBindTextureEXT(GLenum target, GLuint texture)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=2,
    error('invalid number of arguments');
end

moglcore( 'glBindTextureEXT', target, texture );

return
