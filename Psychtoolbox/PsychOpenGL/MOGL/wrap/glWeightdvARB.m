function glWeightdvARB( size, weights )

% glWeightdvARB  Interface to OpenGL function glWeightdvARB
%
% usage:  glWeightdvARB( size, weights )
%
% C function:  void glWeightdvARB(GLint size, const GLdouble* weights)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=2,
    error('invalid number of arguments');
end

moglcore( 'glWeightdvARB', size, double(weights) );

return
