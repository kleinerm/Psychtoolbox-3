function glVertexAttribs1dvNV( index, count, v )

% glVertexAttribs1dvNV  Interface to OpenGL function glVertexAttribs1dvNV
%
% usage:  glVertexAttribs1dvNV( index, count, v )
%
% C function:  void glVertexAttribs1dvNV(GLuint index, GLsizei count, const GLdouble* v)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=3,
    error('invalid number of arguments');
end

moglcore( 'glVertexAttribs1dvNV', index, count, double(v) );

return
