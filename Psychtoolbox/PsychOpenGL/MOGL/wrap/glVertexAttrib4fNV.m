function glVertexAttrib4fNV( index, x, y, z, w )

% glVertexAttrib4fNV  Interface to OpenGL function glVertexAttrib4fNV
%
% usage:  glVertexAttrib4fNV( index, x, y, z, w )
%
% C function:  void glVertexAttrib4fNV(GLuint index, GLfloat x, GLfloat y, GLfloat z, GLfloat w)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=5,
    error('invalid number of arguments');
end

moglcore( 'glVertexAttrib4fNV', index, x, y, z, w );

return
