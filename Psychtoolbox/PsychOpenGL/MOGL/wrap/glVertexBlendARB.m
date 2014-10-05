function glVertexBlendARB( count )

% glVertexBlendARB  Interface to OpenGL function glVertexBlendARB
%
% usage:  glVertexBlendARB( count )
%
% C function:  void glVertexBlendARB(GLint count)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=1,
    error('invalid number of arguments');
end

moglcore( 'glVertexBlendARB', count );

return
