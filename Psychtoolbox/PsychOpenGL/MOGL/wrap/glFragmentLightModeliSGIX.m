function glFragmentLightModeliSGIX( pname, param )

% glFragmentLightModeliSGIX  Interface to OpenGL function glFragmentLightModeliSGIX
%
% usage:  glFragmentLightModeliSGIX( pname, param )
%
% C function:  void glFragmentLightModeliSGIX(GLenum pname, GLint param)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=2,
    error('invalid number of arguments');
end

moglcore( 'glFragmentLightModeliSGIX', pname, param );

return
