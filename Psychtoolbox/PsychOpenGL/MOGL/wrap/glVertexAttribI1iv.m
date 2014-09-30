function glVertexAttribI1iv( index, v )

% glVertexAttribI1iv  Interface to OpenGL function glVertexAttribI1iv
%
% usage:  glVertexAttribI1iv( index, v )
%
% C function:  void glVertexAttribI1iv(GLuint index, const GLint* v)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=2,
    error('invalid number of arguments');
end

moglcore( 'glVertexAttribI1iv', index, int32(v) );

return
