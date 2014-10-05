function glCompileShaderARB( shaderObj )

% glCompileShaderARB  Interface to OpenGL function glCompileShaderARB
%
% usage:  glCompileShaderARB( shaderObj )
%
% C function:  void glCompileShaderARB(GLhandleARB shaderObj)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=1,
    error('invalid number of arguments');
end

moglcore( 'glCompileShaderARB', shaderObj );

return
