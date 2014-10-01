function glVertexAttribs4dvNV( index, count, v )

% glVertexAttribs4dvNV  Interface to OpenGL function glVertexAttribs4dvNV
%
% usage:  glVertexAttribs4dvNV( index, count, v )
%
% C function:  void glVertexAttribs4dvNV(GLuint index, GLsizei count, const GLdouble* v)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=3,
    error('invalid number of arguments');
end

moglcore( 'glVertexAttribs4dvNV', index, count, double(v) );

return
