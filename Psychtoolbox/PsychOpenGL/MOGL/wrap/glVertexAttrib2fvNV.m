function glVertexAttrib2fvNV( index, v )

% glVertexAttrib2fvNV  Interface to OpenGL function glVertexAttrib2fvNV
%
% usage:  glVertexAttrib2fvNV( index, v )
%
% C function:  void glVertexAttrib2fvNV(GLuint index, const GLfloat* v)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=2,
    error('invalid number of arguments');
end

moglcore( 'glVertexAttrib2fvNV', index, single(v) );

return
