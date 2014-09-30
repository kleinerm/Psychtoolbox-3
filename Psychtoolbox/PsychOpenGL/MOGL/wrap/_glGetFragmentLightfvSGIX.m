function params = glGetFragmentLightfvSGIX( light, pname )

% glGetFragmentLightfvSGIX  Interface to OpenGL function glGetFragmentLightfvSGIX
%
% usage:  params = glGetFragmentLightfvSGIX( light, pname )
%
% C function:  void glGetFragmentLightfvSGIX(GLenum light, GLenum pname, GLfloat* params)

% 30-Sep-2014 -- created (generated automatically from header files)

% ---allocate---

if nargin~=2,
    error('invalid number of arguments');
end

params = single(0);

moglcore( 'glGetFragmentLightfvSGIX', light, pname, params );

return
