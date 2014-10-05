function glVertexAttribL2ui64NV( index, x, y )

% glVertexAttribL2ui64NV  Interface to OpenGL function glVertexAttribL2ui64NV
%
% usage:  glVertexAttribL2ui64NV( index, x, y )
%
% C function:  void glVertexAttribL2ui64NV(GLuint index, GLuint64EXT x, GLuint64EXT y)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=3,
    error('invalid number of arguments');
end

moglcore( 'glVertexAttribL2ui64NV', index, x, y );

return
