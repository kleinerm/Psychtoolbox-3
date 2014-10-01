function params = glGetFragmentLightivSGIX( light, pname )

% glGetFragmentLightivSGIX  Interface to OpenGL function glGetFragmentLightivSGIX
%
% usage:  params = glGetFragmentLightivSGIX( light, pname )
%
% C function:  void glGetFragmentLightivSGIX(GLenum light, GLenum pname, GLint* params)

% 30-Sep-2014 -- created (generated automatically from header files)

% ---allocate---

if nargin~=2,
    error('invalid number of arguments');
end

params = int32(0);

moglcore( 'glGetFragmentLightivSGIX', light, pname, params );

return
