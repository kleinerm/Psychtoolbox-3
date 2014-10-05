function glApplyTextureEXT( mode )

% glApplyTextureEXT  Interface to OpenGL function glApplyTextureEXT
%
% usage:  glApplyTextureEXT( mode )
%
% C function:  void glApplyTextureEXT(GLenum mode)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=1,
    error('invalid number of arguments');
end

moglcore( 'glApplyTextureEXT', mode );

return
