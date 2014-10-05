function glWeightbvARB( size, weights )

% glWeightbvARB  Interface to OpenGL function glWeightbvARB
%
% usage:  glWeightbvARB( size, weights )
%
% C function:  void glWeightbvARB(GLint size, const GLbyte* weights)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=2,
    error('invalid number of arguments');
end

moglcore( 'glWeightbvARB', size, int8(weights) );

return
