function glWeightfvARB( size, weights )

% glWeightfvARB  Interface to OpenGL function glWeightfvARB
%
% usage:  glWeightfvARB( size, weights )
%
% C function:  void glWeightfvARB(GLint size, const GLfloat* weights)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=2,
    error('invalid number of arguments');
end

moglcore( 'glWeightfvARB', size, single(weights) );

return
