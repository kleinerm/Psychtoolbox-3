function glBindMultiTextureEXT( texunit, target, texture )

% glBindMultiTextureEXT  Interface to OpenGL function glBindMultiTextureEXT
%
% usage:  glBindMultiTextureEXT( texunit, target, texture )
%
% C function:  void glBindMultiTextureEXT(GLenum texunit, GLenum target, GLuint texture)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=3,
    error('invalid number of arguments');
end

moglcore( 'glBindMultiTextureEXT', texunit, target, texture );

return
