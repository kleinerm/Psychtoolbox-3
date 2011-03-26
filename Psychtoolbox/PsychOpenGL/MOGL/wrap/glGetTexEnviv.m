function params = glGetTexEnviv( target, pname )

% glGetTexEnviv  Interface to OpenGL function glGetTexEnviv
%
% usage:  params = glGetTexEnviv( target, pname )
%
% C function:  void glGetTexEnviv(GLenum target, GLenum pname, GLint* params)

% 24-Jan-2006 -- created (generated automatically from header files)

% ---allocate---
% ---protected---

if nargin~=2,
    error('invalid number of arguments');
end

% a hack to find out how many values are returned
f=glGetTexEnvfv(target,pname);

params = int32(zeros(size(f)));
moglcore( 'glGetTexEnviv', target, pname, params );

return
% ---skip---
