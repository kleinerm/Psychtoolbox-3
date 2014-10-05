function glLoadTransposeMatrixdARB( m )

% glLoadTransposeMatrixdARB  Interface to OpenGL function glLoadTransposeMatrixdARB
%
% usage:  glLoadTransposeMatrixdARB( m )
%
% C function:  void glLoadTransposeMatrixdARB(const GLdouble* m)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=1,
    error('invalid number of arguments');
end

moglcore( 'glLoadTransposeMatrixdARB', double(m) );

return
