function value = glGetPathParameterfvNV( path, pname )

% glGetPathParameterfvNV  Interface to OpenGL function glGetPathParameterfvNV
%
% usage:  value = glGetPathParameterfvNV( path, pname )
%
% C function:  void glGetPathParameterfvNV(GLuint path, GLenum pname, GLfloat* value)

% 30-Sep-2014 -- created (generated automatically from header files)

% ---allocate---

if nargin~=2,
    error('invalid number of arguments');
end

value = single(0);

moglcore( 'glGetPathParameterfvNV', path, pname, value );

return
