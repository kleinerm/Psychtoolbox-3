function params = glGetLightfv( light, pname )

% glGetLightfv  Interface to OpenGL function glGetLightfv
%
% usage:  params = glGetLightfv( light, pname )
%
% C function:  void glGetLightfv(GLenum light, GLenum pname, GLfloat* params)

% 05-Mar-2006 -- created (generated automatically from header files)

% ---allocate---

if nargin~=2,
    error('invalid number of arguments');
end

params = single(0);

moglcore( 'glGetLightfv', light, pname, params );

return
