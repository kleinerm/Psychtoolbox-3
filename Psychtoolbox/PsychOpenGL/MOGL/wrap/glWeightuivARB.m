function glWeightuivARB( size, weights )

% glWeightuivARB  Interface to OpenGL function glWeightuivARB
%
% usage:  glWeightuivARB( size, weights )
%
% C function:  void glWeightuivARB(GLint size, const GLuint* weights)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=2,
    error('invalid number of arguments');
end

moglcore( 'glWeightuivARB', size, uint32(weights) );

return
