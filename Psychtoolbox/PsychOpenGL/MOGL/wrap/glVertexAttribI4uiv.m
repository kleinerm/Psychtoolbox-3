function glVertexAttribI4uiv( index, v )

% glVertexAttribI4uiv  Interface to OpenGL function glVertexAttribI4uiv
%
% usage:  glVertexAttribI4uiv( index, v )
%
% C function:  void glVertexAttribI4uiv(GLuint index, const GLuint* v)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=2,
    error('invalid number of arguments');
end

moglcore( 'glVertexAttribI4uiv', index, uint32(v) );

return
