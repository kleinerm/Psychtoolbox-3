function glFragmentMaterialfvSGIX( face, pname, params )

% glFragmentMaterialfvSGIX  Interface to OpenGL function glFragmentMaterialfvSGIX
%
% usage:  glFragmentMaterialfvSGIX( face, pname, params )
%
% C function:  void glFragmentMaterialfvSGIX(GLenum face, GLenum pname, const GLfloat* params)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=3,
    error('invalid number of arguments');
end

moglcore( 'glFragmentMaterialfvSGIX', face, pname, single(params) );

return
