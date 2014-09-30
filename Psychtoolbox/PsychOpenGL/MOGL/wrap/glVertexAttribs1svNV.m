function glVertexAttribs1svNV( index, count, v )

% glVertexAttribs1svNV  Interface to OpenGL function glVertexAttribs1svNV
%
% usage:  glVertexAttribs1svNV( index, count, v )
%
% C function:  void glVertexAttribs1svNV(GLuint index, GLsizei count, const GLshort* v)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=3,
    error('invalid number of arguments');
end

moglcore( 'glVertexAttribs1svNV', index, count, int16(v) );

return
