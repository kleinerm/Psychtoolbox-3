function glVertexAttribI2iv( index, v )

% glVertexAttribI2iv  Interface to OpenGL function glVertexAttribI2iv
%
% usage:  glVertexAttribI2iv( index, v )
%
% C function:  void glVertexAttribI2iv(GLuint index, const GLint* v)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=2,
    error('invalid number of arguments');
end

moglcore( 'glVertexAttribI2iv', index, int32(v) );

return
