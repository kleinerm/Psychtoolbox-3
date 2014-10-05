function glVertexAttribs1fvNV( index, count, v )

% glVertexAttribs1fvNV  Interface to OpenGL function glVertexAttribs1fvNV
%
% usage:  glVertexAttribs1fvNV( index, count, v )
%
% C function:  void glVertexAttribs1fvNV(GLuint index, GLsizei count, const GLfloat* v)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=3,
    error('invalid number of arguments');
end

moglcore( 'glVertexAttribs1fvNV', index, count, single(v) );

return
