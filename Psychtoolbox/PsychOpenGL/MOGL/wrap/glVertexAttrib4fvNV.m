function glVertexAttrib4fvNV( index, v )

% glVertexAttrib4fvNV  Interface to OpenGL function glVertexAttrib4fvNV
%
% usage:  glVertexAttrib4fvNV( index, v )
%
% C function:  void glVertexAttrib4fvNV(GLuint index, const GLfloat* v)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=2,
    error('invalid number of arguments');
end

moglcore( 'glVertexAttrib4fvNV', index, single(v) );

return
