function glWeightivARB( size, weights )

% glWeightivARB  Interface to OpenGL function glWeightivARB
%
% usage:  glWeightivARB( size, weights )
%
% C function:  void glWeightivARB(GLint size, const GLint* weights)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=2,
    error('invalid number of arguments');
end

moglcore( 'glWeightivARB', size, int32(weights) );

return
