function glMatrixIndexubvARB( size, indices )

% glMatrixIndexubvARB  Interface to OpenGL function glMatrixIndexubvARB
%
% usage:  glMatrixIndexubvARB( size, indices )
%
% C function:  void glMatrixIndexubvARB(GLint size, const GLubyte* indices)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=2,
    error('invalid number of arguments');
end

moglcore( 'glMatrixIndexubvARB', size, uint8(indices) );

return
