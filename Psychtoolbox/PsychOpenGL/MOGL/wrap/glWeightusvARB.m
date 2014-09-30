function glWeightusvARB( size, weights )

% glWeightusvARB  Interface to OpenGL function glWeightusvARB
%
% usage:  glWeightusvARB( size, weights )
%
% C function:  void glWeightusvARB(GLint size, const GLushort* weights)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=2,
    error('invalid number of arguments');
end

moglcore( 'glWeightusvARB', size, uint16(weights) );

return
