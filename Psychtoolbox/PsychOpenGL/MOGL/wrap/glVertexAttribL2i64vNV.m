function glVertexAttribL2i64vNV( index, v )

% glVertexAttribL2i64vNV  Interface to OpenGL function glVertexAttribL2i64vNV
%
% usage:  glVertexAttribL2i64vNV( index, v )
%
% C function:  void glVertexAttribL2i64vNV(GLuint index, const GLint64EXT* v)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=2,
    error('invalid number of arguments');
end

moglcore( 'glVertexAttribL2i64vNV', index, int64(v) );

return
