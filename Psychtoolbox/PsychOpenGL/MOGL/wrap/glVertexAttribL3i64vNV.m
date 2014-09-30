function glVertexAttribL3i64vNV( index, v )

% glVertexAttribL3i64vNV  Interface to OpenGL function glVertexAttribL3i64vNV
%
% usage:  glVertexAttribL3i64vNV( index, v )
%
% C function:  void glVertexAttribL3i64vNV(GLuint index, const GLint64EXT* v)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=2,
    error('invalid number of arguments');
end

moglcore( 'glVertexAttribL3i64vNV', index, int64(v) );

return
