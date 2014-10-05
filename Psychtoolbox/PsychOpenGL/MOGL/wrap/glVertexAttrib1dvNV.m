function glVertexAttrib1dvNV( index, v )

% glVertexAttrib1dvNV  Interface to OpenGL function glVertexAttrib1dvNV
%
% usage:  glVertexAttrib1dvNV( index, v )
%
% C function:  void glVertexAttrib1dvNV(GLuint index, const GLdouble* v)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=2,
    error('invalid number of arguments');
end

moglcore( 'glVertexAttrib1dvNV', index, double(v) );

return
