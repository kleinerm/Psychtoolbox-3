function glVertexAttrib3fNV( index, x, y, z )

% glVertexAttrib3fNV  Interface to OpenGL function glVertexAttrib3fNV
%
% usage:  glVertexAttrib3fNV( index, x, y, z )
%
% C function:  void glVertexAttrib3fNV(GLuint index, GLfloat x, GLfloat y, GLfloat z)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=4,
    error('invalid number of arguments');
end

moglcore( 'glVertexAttrib3fNV', index, x, y, z );

return
