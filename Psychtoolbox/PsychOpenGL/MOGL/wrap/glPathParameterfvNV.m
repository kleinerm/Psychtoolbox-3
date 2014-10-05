function glPathParameterfvNV( path, pname, value )

% glPathParameterfvNV  Interface to OpenGL function glPathParameterfvNV
%
% usage:  glPathParameterfvNV( path, pname, value )
%
% C function:  void glPathParameterfvNV(GLuint path, GLenum pname, const GLfloat* value)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=3,
    error('invalid number of arguments');
end

moglcore( 'glPathParameterfvNV', path, pname, single(value) );

return
