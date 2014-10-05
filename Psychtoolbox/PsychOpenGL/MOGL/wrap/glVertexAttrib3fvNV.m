function glVertexAttrib3fvNV( index, v )

% glVertexAttrib3fvNV  Interface to OpenGL function glVertexAttrib3fvNV
%
% usage:  glVertexAttrib3fvNV( index, v )
%
% C function:  void glVertexAttrib3fvNV(GLuint index, const GLfloat* v)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=2,
    error('invalid number of arguments');
end

moglcore( 'glVertexAttrib3fvNV', index, single(v) );

return
