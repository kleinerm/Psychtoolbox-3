function glVertexAttribs3fvNV( index, count, v )

% glVertexAttribs3fvNV  Interface to OpenGL function glVertexAttribs3fvNV
%
% usage:  glVertexAttribs3fvNV( index, count, v )
%
% C function:  void glVertexAttribs3fvNV(GLuint index, GLsizei count, const GLfloat* v)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=3,
    error('invalid number of arguments');
end

moglcore( 'glVertexAttribs3fvNV', index, count, single(v) );

return
