function glTextureMaterialEXT( face, mode )

% glTextureMaterialEXT  Interface to OpenGL function glTextureMaterialEXT
%
% usage:  glTextureMaterialEXT( face, mode )
%
% C function:  void glTextureMaterialEXT(GLenum face, GLenum mode)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=2,
    error('invalid number of arguments');
end

moglcore( 'glTextureMaterialEXT', face, mode );

return
