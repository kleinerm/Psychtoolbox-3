function glVertexAttribL1i64NV( index, x )

% glVertexAttribL1i64NV  Interface to OpenGL function glVertexAttribL1i64NV
%
% usage:  glVertexAttribL1i64NV( index, x )
%
% C function:  void glVertexAttribL1i64NV(GLuint index, GLint64EXT x)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=2,
    error('invalid number of arguments');
end

moglcore( 'glVertexAttribL1i64NV', index, x );

return
