function glVertexAttribs2svNV( index, count, v )

% glVertexAttribs2svNV  Interface to OpenGL function glVertexAttribs2svNV
%
% usage:  glVertexAttribs2svNV( index, count, v )
%
% C function:  void glVertexAttribs2svNV(GLuint index, GLsizei count, const GLshort* v)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=3,
    error('invalid number of arguments');
end

moglcore( 'glVertexAttribs2svNV', index, count, int16(v) );

return
