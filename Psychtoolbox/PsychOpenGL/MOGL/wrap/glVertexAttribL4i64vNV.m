function glVertexAttribL4i64vNV( index, v )

% glVertexAttribL4i64vNV  Interface to OpenGL function glVertexAttribL4i64vNV
%
% usage:  glVertexAttribL4i64vNV( index, v )
%
% C function:  void glVertexAttribL4i64vNV(GLuint index, const GLint64EXT* v)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=2,
    error('invalid number of arguments');
end

moglcore( 'glVertexAttribL4i64vNV', index, int64(v) );

return
