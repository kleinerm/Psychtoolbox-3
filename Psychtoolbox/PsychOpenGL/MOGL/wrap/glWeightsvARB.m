function glWeightsvARB( size, weights )

% glWeightsvARB  Interface to OpenGL function glWeightsvARB
%
% usage:  glWeightsvARB( size, weights )
%
% C function:  void glWeightsvARB(GLint size, const GLshort* weights)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=2,
    error('invalid number of arguments');
end

moglcore( 'glWeightsvARB', size, int16(weights) );

return
