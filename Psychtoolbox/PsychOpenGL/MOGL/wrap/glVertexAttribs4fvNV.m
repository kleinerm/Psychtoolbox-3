function glVertexAttribs4fvNV( index, count, v )

% glVertexAttribs4fvNV  Interface to OpenGL function glVertexAttribs4fvNV
%
% usage:  glVertexAttribs4fvNV( index, count, v )
%
% C function:  void glVertexAttribs4fvNV(GLuint index, GLsizei count, const GLfloat* v)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=3,
    error('invalid number of arguments');
end

moglcore( 'glVertexAttribs4fvNV', index, count, single(v) );

return
