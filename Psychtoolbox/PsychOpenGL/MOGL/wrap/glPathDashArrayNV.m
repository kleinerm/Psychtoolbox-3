function glPathDashArrayNV( path, dashCount, dashArray )

% glPathDashArrayNV  Interface to OpenGL function glPathDashArrayNV
%
% usage:  glPathDashArrayNV( path, dashCount, dashArray )
%
% C function:  void glPathDashArrayNV(GLuint path, GLsizei dashCount, const GLfloat* dashArray)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=3,
    error('invalid number of arguments');
end

moglcore( 'glPathDashArrayNV', path, dashCount, single(dashArray) );

return
