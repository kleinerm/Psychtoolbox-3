function glVertexAttribI4iv( index, v )

% glVertexAttribI4iv  Interface to OpenGL function glVertexAttribI4iv
%
% usage:  glVertexAttribI4iv( index, v )
%
% C function:  void glVertexAttribI4iv(GLuint index, const GLint* v)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=2,
    error('invalid number of arguments');
end

moglcore( 'glVertexAttribI4iv', index, int32(v) );

return
