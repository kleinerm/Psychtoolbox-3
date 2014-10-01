function glVertexAttribI2uiv( index, v )

% glVertexAttribI2uiv  Interface to OpenGL function glVertexAttribI2uiv
%
% usage:  glVertexAttribI2uiv( index, v )
%
% C function:  void glVertexAttribI2uiv(GLuint index, const GLuint* v)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=2,
    error('invalid number of arguments');
end

moglcore( 'glVertexAttribI2uiv', index, uint32(v) );

return
