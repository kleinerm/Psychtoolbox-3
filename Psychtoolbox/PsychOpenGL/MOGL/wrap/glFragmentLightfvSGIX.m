function glFragmentLightfvSGIX( light, pname, params )

% glFragmentLightfvSGIX  Interface to OpenGL function glFragmentLightfvSGIX
%
% usage:  glFragmentLightfvSGIX( light, pname, params )
%
% C function:  void glFragmentLightfvSGIX(GLenum light, GLenum pname, const GLfloat* params)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=3,
    error('invalid number of arguments');
end

moglcore( 'glFragmentLightfvSGIX', light, pname, single(params) );

return
