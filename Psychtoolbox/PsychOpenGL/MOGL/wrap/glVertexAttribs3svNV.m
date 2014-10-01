function glVertexAttribs3svNV( index, count, v )

% glVertexAttribs3svNV  Interface to OpenGL function glVertexAttribs3svNV
%
% usage:  glVertexAttribs3svNV( index, count, v )
%
% C function:  void glVertexAttribs3svNV(GLuint index, GLsizei count, const GLshort* v)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=3,
    error('invalid number of arguments');
end

moglcore( 'glVertexAttribs3svNV', index, count, int16(v) );

return
