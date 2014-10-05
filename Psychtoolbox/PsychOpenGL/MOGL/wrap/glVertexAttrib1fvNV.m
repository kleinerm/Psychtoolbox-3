function glVertexAttrib1fvNV( index, v )

% glVertexAttrib1fvNV  Interface to OpenGL function glVertexAttrib1fvNV
%
% usage:  glVertexAttrib1fvNV( index, v )
%
% C function:  void glVertexAttrib1fvNV(GLuint index, const GLfloat* v)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=2,
    error('invalid number of arguments');
end

moglcore( 'glVertexAttrib1fvNV', index, single(v) );

return
