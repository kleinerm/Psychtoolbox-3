function glVertexAttribL1ui64NV( index, x )

% glVertexAttribL1ui64NV  Interface to OpenGL function glVertexAttribL1ui64NV
%
% usage:  glVertexAttribL1ui64NV( index, x )
%
% C function:  void glVertexAttribL1ui64NV(GLuint index, GLuint64EXT x)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=2,
    error('invalid number of arguments');
end

moglcore( 'glVertexAttribL1ui64NV', index, x );

return
