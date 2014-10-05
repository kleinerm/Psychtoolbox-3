function glVertexAttrib2fNV( index, x, y )

% glVertexAttrib2fNV  Interface to OpenGL function glVertexAttrib2fNV
%
% usage:  glVertexAttrib2fNV( index, x, y )
%
% C function:  void glVertexAttrib2fNV(GLuint index, GLfloat x, GLfloat y)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=3,
    error('invalid number of arguments');
end

moglcore( 'glVertexAttrib2fNV', index, x, y );

return
