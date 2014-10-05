function glFragmentLightModelivSGIX( pname, params )

% glFragmentLightModelivSGIX  Interface to OpenGL function glFragmentLightModelivSGIX
%
% usage:  glFragmentLightModelivSGIX( pname, params )
%
% C function:  void glFragmentLightModelivSGIX(GLenum pname, const GLint* params)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=2,
    error('invalid number of arguments');
end

moglcore( 'glFragmentLightModelivSGIX', pname, int32(params) );

return
