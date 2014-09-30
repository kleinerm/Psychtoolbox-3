function params = glGetMapParameterfvNV( target, pname )

% glGetMapParameterfvNV  Interface to OpenGL function glGetMapParameterfvNV
%
% usage:  params = glGetMapParameterfvNV( target, pname )
%
% C function:  void glGetMapParameterfvNV(GLenum target, GLenum pname, GLfloat* params)

% 30-Sep-2014 -- created (generated automatically from header files)

% ---allocate---

if nargin~=2,
    error('invalid number of arguments');
end

params = single(0);

moglcore( 'glGetMapParameterfvNV', target, pname, params );

return
