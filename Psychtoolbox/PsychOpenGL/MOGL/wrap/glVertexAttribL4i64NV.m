function glVertexAttribL4i64NV( index, x, y, z, w )

% glVertexAttribL4i64NV  Interface to OpenGL function glVertexAttribL4i64NV
%
% usage:  glVertexAttribL4i64NV( index, x, y, z, w )
%
% C function:  void glVertexAttribL4i64NV(GLuint index, GLint64EXT x, GLint64EXT y, GLint64EXT z, GLint64EXT w)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=5,
    error('invalid number of arguments');
end

moglcore( 'glVertexAttribL4i64NV', index, x, y, z, w );

return
