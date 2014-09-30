function glWeightubvARB( size, weights )

% glWeightubvARB  Interface to OpenGL function glWeightubvARB
%
% usage:  glWeightubvARB( size, weights )
%
% C function:  void glWeightubvARB(GLint size, const GLubyte* weights)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=2,
    error('invalid number of arguments');
end

moglcore( 'glWeightubvARB', size, uint8(weights) );

return
