function glVertexAttribL1ui64vARB( index, v )

% glVertexAttribL1ui64vARB  Interface to OpenGL function glVertexAttribL1ui64vARB
%
% usage:  glVertexAttribL1ui64vARB( index, v )
%
% C function:  void glVertexAttribL1ui64vARB(GLuint index, const GLuint64EXT* v)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=2,
    error('invalid number of arguments');
end

moglcore( 'glVertexAttribL1ui64vARB', index, uint64(v) );

return
