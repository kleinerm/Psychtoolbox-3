function glVertexAttribs4ubvNV( index, count, v )

% glVertexAttribs4ubvNV  Interface to OpenGL function glVertexAttribs4ubvNV
%
% usage:  glVertexAttribs4ubvNV( index, count, v )
%
% C function:  void glVertexAttribs4ubvNV(GLuint index, GLsizei count, const GLubyte* v)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=3,
    error('invalid number of arguments');
end

moglcore( 'glVertexAttribs4ubvNV', index, count, uint8(v) );

return
