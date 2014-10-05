function params = glGetColorTableParameterfvSGI( target, pname )

% glGetColorTableParameterfvSGI  Interface to OpenGL function glGetColorTableParameterfvSGI
%
% usage:  params = glGetColorTableParameterfvSGI( target, pname )
%
% C function:  void glGetColorTableParameterfvSGI(GLenum target, GLenum pname, GLfloat* params)

% 30-Sep-2014 -- created (generated automatically from header files)

% ---allocate---

if nargin~=2,
    error('invalid number of arguments');
end

params = single(0);

moglcore( 'glGetColorTableParameterfvSGI', target, pname, params );

return
