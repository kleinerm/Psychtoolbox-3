function glFragmentColorMaterialSGIX( face, mode )

% glFragmentColorMaterialSGIX  Interface to OpenGL function glFragmentColorMaterialSGIX
%
% usage:  glFragmentColorMaterialSGIX( face, mode )
%
% C function:  void glFragmentColorMaterialSGIX(GLenum face, GLenum mode)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=2,
    error('invalid number of arguments');
end

moglcore( 'glFragmentColorMaterialSGIX', face, mode );

return
