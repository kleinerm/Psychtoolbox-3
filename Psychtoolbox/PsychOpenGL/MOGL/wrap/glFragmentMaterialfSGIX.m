function glFragmentMaterialfSGIX( face, pname, param )

% glFragmentMaterialfSGIX  Interface to OpenGL function glFragmentMaterialfSGIX
%
% usage:  glFragmentMaterialfSGIX( face, pname, param )
%
% C function:  void glFragmentMaterialfSGIX(GLenum face, GLenum pname, GLfloat param)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=3,
    error('invalid number of arguments');
end

moglcore( 'glFragmentMaterialfSGIX', face, pname, param );

return
