function glPathParameterfNV( path, pname, value )

% glPathParameterfNV  Interface to OpenGL function glPathParameterfNV
%
% usage:  glPathParameterfNV( path, pname, value )
%
% C function:  void glPathParameterfNV(GLuint path, GLenum pname, GLfloat value)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=3,
    error('invalid number of arguments');
end

moglcore( 'glPathParameterfNV', path, pname, value );

return
