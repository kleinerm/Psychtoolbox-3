function params = glGetMapAttribParameterfvNV( target, index, pname )

% glGetMapAttribParameterfvNV  Interface to OpenGL function glGetMapAttribParameterfvNV
%
% usage:  params = glGetMapAttribParameterfvNV( target, index, pname )
%
% C function:  void glGetMapAttribParameterfvNV(GLenum target, GLuint index, GLenum pname, GLfloat* params)

% 30-Sep-2014 -- created (generated automatically from header files)

% ---allocate---

if nargin~=3,
    error('invalid number of arguments');
end

params = single(0);

moglcore( 'glGetMapAttribParameterfvNV', target, index, pname, params );

return
