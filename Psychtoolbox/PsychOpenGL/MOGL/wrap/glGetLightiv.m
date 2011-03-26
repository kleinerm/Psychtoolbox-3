function params = glGetLightiv( light, pname )

% glGetLightiv  Interface to OpenGL function glGetLightiv
%
% usage:  params = glGetLightiv( light, pname )
%
% C function:  void glGetLightiv(GLenum light, GLenum pname, GLint* params)

% 24-Jan-2006 -- created (generated automatically from header files)

% ---allocate---
% ---protected---

if nargin~=2,
    error('invalid number of arguments');
end

% a hack to find out how many values are returned
f=glGetLightfv(light,pname);

params = int32(zeros(size(f)));
moglcore( 'glGetLightiv', light, pname, params );

return
% ---skip---
