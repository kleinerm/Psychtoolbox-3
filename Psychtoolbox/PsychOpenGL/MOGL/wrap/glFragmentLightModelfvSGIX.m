function glFragmentLightModelfvSGIX( pname, params )

% glFragmentLightModelfvSGIX  Interface to OpenGL function glFragmentLightModelfvSGIX
%
% usage:  glFragmentLightModelfvSGIX( pname, params )
%
% C function:  void glFragmentLightModelfvSGIX(GLenum pname, const GLfloat* params)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=2,
    error('invalid number of arguments');
end

moglcore( 'glFragmentLightModelfvSGIX', pname, single(params) );

return
