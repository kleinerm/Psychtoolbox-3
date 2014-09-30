function glFragmentLightModelfSGIX( pname, param )

% glFragmentLightModelfSGIX  Interface to OpenGL function glFragmentLightModelfSGIX
%
% usage:  glFragmentLightModelfSGIX( pname, param )
%
% C function:  void glFragmentLightModelfSGIX(GLenum pname, GLfloat param)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=2,
    error('invalid number of arguments');
end

moglcore( 'glFragmentLightModelfSGIX', pname, param );

return
