function glFragmentMaterialivSGIX( face, pname, params )

% glFragmentMaterialivSGIX  Interface to OpenGL function glFragmentMaterialivSGIX
%
% usage:  glFragmentMaterialivSGIX( face, pname, params )
%
% C function:  void glFragmentMaterialivSGIX(GLenum face, GLenum pname, const GLint* params)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=3,
    error('invalid number of arguments');
end

moglcore( 'glFragmentMaterialivSGIX', face, pname, int32(params) );

return
