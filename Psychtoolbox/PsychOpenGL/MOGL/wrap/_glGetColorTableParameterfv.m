function params = glGetColorTableParameterfv( target, pname )

% glGetColorTableParameterfv  Interface to OpenGL function glGetColorTableParameterfv
%
% usage:  params = glGetColorTableParameterfv( target, pname )
%
% C function:  void glGetColorTableParameterfv(GLenum target, GLenum pname, GLfloat* params)

% 25-Mar-2011 -- created (generated automatically from header files)

% ---allocate---

if nargin~=2,
    error('invalid number of arguments');
end

params = single(0);

moglcore( 'glGetColorTableParameterfv', target, pname, params );

return
