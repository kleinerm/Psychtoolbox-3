function glLoadTransposeMatrixfARB( m )

% glLoadTransposeMatrixfARB  Interface to OpenGL function glLoadTransposeMatrixfARB
%
% usage:  glLoadTransposeMatrixfARB( m )
%
% C function:  void glLoadTransposeMatrixfARB(const GLfloat* m)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=1,
    error('invalid number of arguments');
end

moglcore( 'glLoadTransposeMatrixfARB', single(m) );

return
