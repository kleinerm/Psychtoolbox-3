function glVertexAttribL1ui64ARB( index, x )

% glVertexAttribL1ui64ARB  Interface to OpenGL function glVertexAttribL1ui64ARB
%
% usage:  glVertexAttribL1ui64ARB( index, x )
%
% C function:  void glVertexAttribL1ui64ARB(GLuint index, GLuint64EXT x)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=2,
    error('invalid number of arguments');
end

moglcore( 'glVertexAttribL1ui64ARB', index, x );

return
