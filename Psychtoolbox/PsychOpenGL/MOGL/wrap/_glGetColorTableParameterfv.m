function params = glGetColorTableParameterfv( target, pname )

% glGetColorTableParameterfv  Interface to OpenGL function glGetColorTableParameterfv
%
% usage:  params = glGetColorTableParameterfv( target, pname )
%
% C function:  void glGetColorTableParameterfv(GLenum target, GLenum pname, GLfloat* params)

% 05-Mar-2006 -- created (generated automatically from header files)

% ---allocate---

if nargin~=2,
    error('invalid number of arguments');
end

params = moglsingle(0);

moglcore( 'glGetColorTableParameterfv', target, pname, params );

return
