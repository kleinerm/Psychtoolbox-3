function glFragmentLightfSGIX( light, pname, param )

% glFragmentLightfSGIX  Interface to OpenGL function glFragmentLightfSGIX
%
% usage:  glFragmentLightfSGIX( light, pname, param )
%
% C function:  void glFragmentLightfSGIX(GLenum light, GLenum pname, GLfloat param)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=3,
    error('invalid number of arguments');
end

moglcore( 'glFragmentLightfSGIX', light, pname, param );

return
