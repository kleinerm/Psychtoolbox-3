function glVertexAttribI3iv( index, v )

% glVertexAttribI3iv  Interface to OpenGL function glVertexAttribI3iv
%
% usage:  glVertexAttribI3iv( index, v )
%
% C function:  void glVertexAttribI3iv(GLuint index, const GLint* v)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=2,
    error('invalid number of arguments');
end

moglcore( 'glVertexAttribI3iv', index, int32(v) );

return
