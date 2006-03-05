function params = glGetTexParameterfv( target, pname )

% glGetTexParameterfv  Interface to OpenGL function glGetTexParameterfv
%
% usage:  params = glGetTexParameterfv( target, pname )
%
% C function:  void glGetTexParameterfv(GLenum target, GLenum pname, GLfloat* params)

% 05-Mar-2006 -- created (generated automatically from header files)

% ---allocate---

if nargin~=2,
    error('invalid number of arguments');
end

params = single(0);

moglcore( 'glGetTexParameterfv', target, pname, params );

return
