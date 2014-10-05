function glMultTransposeMatrixdARB( m )

% glMultTransposeMatrixdARB  Interface to OpenGL function glMultTransposeMatrixdARB
%
% usage:  glMultTransposeMatrixdARB( m )
%
% C function:  void glMultTransposeMatrixdARB(const GLdouble* m)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=1,
    error('invalid number of arguments');
end

moglcore( 'glMultTransposeMatrixdARB', double(m) );

return
