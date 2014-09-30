function glVertexAttribs4svNV( index, count, v )

% glVertexAttribs4svNV  Interface to OpenGL function glVertexAttribs4svNV
%
% usage:  glVertexAttribs4svNV( index, count, v )
%
% C function:  void glVertexAttribs4svNV(GLuint index, GLsizei count, const GLshort* v)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=3,
    error('invalid number of arguments');
end

moglcore( 'glVertexAttribs4svNV', index, count, int16(v) );

return
