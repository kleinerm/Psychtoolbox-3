function glFragmentLightivSGIX( light, pname, params )

% glFragmentLightivSGIX  Interface to OpenGL function glFragmentLightivSGIX
%
% usage:  glFragmentLightivSGIX( light, pname, params )
%
% C function:  void glFragmentLightivSGIX(GLenum light, GLenum pname, const GLint* params)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=3,
    error('invalid number of arguments');
end

moglcore( 'glFragmentLightivSGIX', light, pname, int32(params) );

return
