function glVertexAttribI3uiv( index, v )

% glVertexAttribI3uiv  Interface to OpenGL function glVertexAttribI3uiv
%
% usage:  glVertexAttribI3uiv( index, v )
%
% C function:  void glVertexAttribI3uiv(GLuint index, const GLuint* v)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=2,
    error('invalid number of arguments');
end

moglcore( 'glVertexAttribI3uiv', index, uint32(v) );

return
