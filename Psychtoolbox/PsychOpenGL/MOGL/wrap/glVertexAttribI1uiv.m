function glVertexAttribI1uiv( index, v )

% glVertexAttribI1uiv  Interface to OpenGL function glVertexAttribI1uiv
%
% usage:  glVertexAttribI1uiv( index, v )
%
% C function:  void glVertexAttribI1uiv(GLuint index, const GLuint* v)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=2,
    error('invalid number of arguments');
end

moglcore( 'glVertexAttribI1uiv', index, uint32(v) );

return
