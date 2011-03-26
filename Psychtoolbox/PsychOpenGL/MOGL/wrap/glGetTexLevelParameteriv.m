function params = glGetTexLevelParameteriv( target, level, pname )

% glGetTexLevelParameteriv  Interface to OpenGL function glGetTexLevelParameteriv
%
% usage:  params = glGetTexLevelParameteriv( target, level, pname )
%
% C function:  void glGetTexLevelParameteriv(GLenum target, GLint level, GLenum pname, GLint* params)

% 05-Mar-2006 -- created (generated automatically from header files)

% ---allocate---
% ---protected---
% ---skip---

if nargin~=3,
    error('invalid number of arguments');
end

params = int32(zeros(2,1));

moglcore( 'glGetTexLevelParameteriv', target, level, pname, params );
params = params(1,1);

return
