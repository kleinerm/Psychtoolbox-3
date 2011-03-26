function params = glGetTexLevelParameterfv( target, level, pname )

% glGetTexLevelParameterfv  Interface to OpenGL function glGetTexLevelParameterfv
%
% usage:  params = glGetTexLevelParameterfv( target, level, pname )
%
% C function:  void glGetTexLevelParameterfv(GLenum target, GLint level, GLenum pname, GLfloat* params)

% 05-Mar-2006 -- created (generated automatically from header files)

% ---allocate---
% ---protected---
% ---skip---

if nargin~=3,
    error('invalid number of arguments');
end

params = single(0);

moglcore( 'glGetTexLevelParameterfv', target, level, pname, params );

return
