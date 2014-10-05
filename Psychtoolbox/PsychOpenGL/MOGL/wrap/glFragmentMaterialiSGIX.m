function glFragmentMaterialiSGIX( face, pname, param )

% glFragmentMaterialiSGIX  Interface to OpenGL function glFragmentMaterialiSGIX
%
% usage:  glFragmentMaterialiSGIX( face, pname, param )
%
% C function:  void glFragmentMaterialiSGIX(GLenum face, GLenum pname, GLint param)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=3,
    error('invalid number of arguments');
end

moglcore( 'glFragmentMaterialiSGIX', face, pname, param );

return
