function glVertexAttribs2fvNV( index, count, v )

% glVertexAttribs2fvNV  Interface to OpenGL function glVertexAttribs2fvNV
%
% usage:  glVertexAttribs2fvNV( index, count, v )
%
% C function:  void glVertexAttribs2fvNV(GLuint index, GLsizei count, const GLfloat* v)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=3,
    error('invalid number of arguments');
end

moglcore( 'glVertexAttribs2fvNV', index, count, single(v) );

return
