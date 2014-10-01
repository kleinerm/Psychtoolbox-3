function glVertexAttrib1dNV( index, x )

% glVertexAttrib1dNV  Interface to OpenGL function glVertexAttrib1dNV
%
% usage:  glVertexAttrib1dNV( index, x )
%
% C function:  void glVertexAttrib1dNV(GLuint index, GLdouble x)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=2,
    error('invalid number of arguments');
end

moglcore( 'glVertexAttrib1dNV', index, x );

return
