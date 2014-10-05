function glVertexAttribs2dvNV( index, count, v )

% glVertexAttribs2dvNV  Interface to OpenGL function glVertexAttribs2dvNV
%
% usage:  glVertexAttribs2dvNV( index, count, v )
%
% C function:  void glVertexAttribs2dvNV(GLuint index, GLsizei count, const GLdouble* v)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=3,
    error('invalid number of arguments');
end

moglcore( 'glVertexAttribs2dvNV', index, count, double(v) );

return
