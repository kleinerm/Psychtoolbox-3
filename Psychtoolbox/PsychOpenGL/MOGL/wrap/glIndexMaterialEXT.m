function glIndexMaterialEXT( face, mode )

% glIndexMaterialEXT  Interface to OpenGL function glIndexMaterialEXT
%
% usage:  glIndexMaterialEXT( face, mode )
%
% C function:  void glIndexMaterialEXT(GLenum face, GLenum mode)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=2,
    error('invalid number of arguments');
end

moglcore( 'glIndexMaterialEXT', face, mode );

return
