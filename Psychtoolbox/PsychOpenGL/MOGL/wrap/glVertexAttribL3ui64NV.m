function glVertexAttribL3ui64NV( index, x, y, z )

% glVertexAttribL3ui64NV  Interface to OpenGL function glVertexAttribL3ui64NV
%
% usage:  glVertexAttribL3ui64NV( index, x, y, z )
%
% C function:  void glVertexAttribL3ui64NV(GLuint index, GLuint64EXT x, GLuint64EXT y, GLuint64EXT z)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=4,
    error('invalid number of arguments');
end

moglcore( 'glVertexAttribL3ui64NV', index, x, y, z );

return
