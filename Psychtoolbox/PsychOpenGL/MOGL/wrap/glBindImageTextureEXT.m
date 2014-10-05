function glBindImageTextureEXT( index, texture, level, layered, layer, access, format )

% glBindImageTextureEXT  Interface to OpenGL function glBindImageTextureEXT
%
% usage:  glBindImageTextureEXT( index, texture, level, layered, layer, access, format )
%
% C function:  void glBindImageTextureEXT(GLuint index, GLuint texture, GLint level, GLboolean layered, GLint layer, GLenum access, GLint format)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=7,
    error('invalid number of arguments');
end

moglcore( 'glBindImageTextureEXT', index, texture, level, layered, layer, access, format );

return
