function glGenerateTextureMipmapEXT( texture, target )

% glGenerateTextureMipmapEXT  Interface to OpenGL function glGenerateTextureMipmapEXT
%
% usage:  glGenerateTextureMipmapEXT( texture, target )
%
% C function:  void glGenerateTextureMipmapEXT(GLuint texture, GLenum target)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=2,
    error('invalid number of arguments');
end

moglcore( 'glGenerateTextureMipmapEXT', texture, target );

return
