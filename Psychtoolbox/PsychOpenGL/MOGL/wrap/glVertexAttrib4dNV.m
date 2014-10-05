function glVertexAttrib4dNV( index, x, y, z, w )

% glVertexAttrib4dNV  Interface to OpenGL function glVertexAttrib4dNV
%
% usage:  glVertexAttrib4dNV( index, x, y, z, w )
%
% C function:  void glVertexAttrib4dNV(GLuint index, GLdouble x, GLdouble y, GLdouble z, GLdouble w)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=5,
    error('invalid number of arguments');
end

moglcore( 'glVertexAttrib4dNV', index, x, y, z, w );

return
