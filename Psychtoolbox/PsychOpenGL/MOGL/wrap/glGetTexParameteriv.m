function params = glGetTexParameteriv( target, pname )

% glGetTexParameteriv  Interface to OpenGL function glGetTexParameteriv
%
% usage:  params = glGetTexParameteriv( target, pname )
%
% C function:  void glGetTexParameteriv(GLenum target, GLenum pname, GLint* params)

% 24-Jan-2006 -- created (generated automatically from header files)

% ---allocate---
% ---protected---

if nargin~=2,
    error('invalid number of arguments');
end

% a hack to find out how many values are returned
f=glGetTexParameterfv(target,pname);

params = int32(zeros(size(f)));
moglcore( 'glGetTexParameteriv', target, pname, params );

return
% ---skip---
