function glVertexAttrib2dNV( index, x, y )

% glVertexAttrib2dNV  Interface to OpenGL function glVertexAttrib2dNV
%
% usage:  glVertexAttrib2dNV( index, x, y )
%
% C function:  void glVertexAttrib2dNV(GLuint index, GLdouble x, GLdouble y)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=3,
    error('invalid number of arguments');
end

moglcore( 'glVertexAttrib2dNV', index, x, y );

return
