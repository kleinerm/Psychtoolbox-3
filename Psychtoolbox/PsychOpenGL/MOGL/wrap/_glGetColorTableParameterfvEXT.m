function params = glGetColorTableParameterfvEXT( target, pname )

% glGetColorTableParameterfvEXT  Interface to OpenGL function glGetColorTableParameterfvEXT
%
% usage:  params = glGetColorTableParameterfvEXT( target, pname )
%
% C function:  void glGetColorTableParameterfvEXT(GLenum target, GLenum pname, GLfloat* params)

% 30-Sep-2014 -- created (generated automatically from header files)

% ---allocate---

if nargin~=2,
    error('invalid number of arguments');
end

params = single(0);

moglcore( 'glGetColorTableParameterfvEXT', target, pname, params );

return
