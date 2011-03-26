function params = glGetTexGeniv( coord, pname )

% glGetTexGeniv  Interface to OpenGL function glGetTexGeniv
%
% usage:  params = glGetTexGeniv( coord, pname )
%
% C function:  void glGetTexGeniv(GLenum coord, GLenum pname, GLint* params)

% 24-Jan-2006 -- created (generated automatically from header files)

% ---allocate---
% ---protected---

if nargin~=2,
    error('invalid number of arguments');
end

% a hack to find out how many values are returned
f=glGetTexGendv(coord,pname);

params = int32(zeros(size(f)));
moglcore( 'glGetTexGeniv', coord, pname, params );

return
% ---skip---
