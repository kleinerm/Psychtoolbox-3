function glMultTransposeMatrixfARB( m )

% glMultTransposeMatrixfARB  Interface to OpenGL function glMultTransposeMatrixfARB
%
% usage:  glMultTransposeMatrixfARB( m )
%
% C function:  void glMultTransposeMatrixfARB(const GLfloat* m)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=1,
    error('invalid number of arguments');
end

moglcore( 'glMultTransposeMatrixfARB', single(m) );

return
