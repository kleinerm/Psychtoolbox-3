function glVertexAttribL2i64NV( index, x, y )

% glVertexAttribL2i64NV  Interface to OpenGL function glVertexAttribL2i64NV
%
% usage:  glVertexAttribL2i64NV( index, x, y )
%
% C function:  void glVertexAttribL2i64NV(GLuint index, GLint64EXT x, GLint64EXT y)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=3,
    error('invalid number of arguments');
end

moglcore( 'glVertexAttribL2i64NV', index, x, y );

return
