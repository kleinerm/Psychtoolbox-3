function glVertexAttribs3dvNV( index, count, v )

% glVertexAttribs3dvNV  Interface to OpenGL function glVertexAttribs3dvNV
%
% usage:  glVertexAttribs3dvNV( index, count, v )
%
% C function:  void glVertexAttribs3dvNV(GLuint index, GLsizei count, const GLdouble* v)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=3,
    error('invalid number of arguments');
end

moglcore( 'glVertexAttribs3dvNV', index, count, double(v) );

return
