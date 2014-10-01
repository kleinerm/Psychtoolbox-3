function glVertexAttrib3dNV( index, x, y, z )

% glVertexAttrib3dNV  Interface to OpenGL function glVertexAttrib3dNV
%
% usage:  glVertexAttrib3dNV( index, x, y, z )
%
% C function:  void glVertexAttrib3dNV(GLuint index, GLdouble x, GLdouble y, GLdouble z)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=4,
    error('invalid number of arguments');
end

moglcore( 'glVertexAttrib3dNV', index, x, y, z );

return
