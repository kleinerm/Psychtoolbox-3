function glVertexAttribL3i64NV( index, x, y, z )

% glVertexAttribL3i64NV  Interface to OpenGL function glVertexAttribL3i64NV
%
% usage:  glVertexAttribL3i64NV( index, x, y, z )
%
% C function:  void glVertexAttribL3i64NV(GLuint index, GLint64EXT x, GLint64EXT y, GLint64EXT z)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=4,
    error('invalid number of arguments');
end

moglcore( 'glVertexAttribL3i64NV', index, x, y, z );

return
