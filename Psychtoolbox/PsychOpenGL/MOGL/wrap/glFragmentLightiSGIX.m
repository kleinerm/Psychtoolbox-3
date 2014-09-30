function glFragmentLightiSGIX( light, pname, param )

% glFragmentLightiSGIX  Interface to OpenGL function glFragmentLightiSGIX
%
% usage:  glFragmentLightiSGIX( light, pname, param )
%
% C function:  void glFragmentLightiSGIX(GLenum light, GLenum pname, GLint param)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=3,
    error('invalid number of arguments');
end

moglcore( 'glFragmentLightiSGIX', light, pname, param );

return
