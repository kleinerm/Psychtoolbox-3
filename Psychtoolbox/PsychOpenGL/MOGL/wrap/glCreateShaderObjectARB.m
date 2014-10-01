function r = glCreateShaderObjectARB( shaderType )

% glCreateShaderObjectARB  Interface to OpenGL function glCreateShaderObjectARB
%
% usage:  r = glCreateShaderObjectARB( shaderType )
%
% C function:  GLhandleARB glCreateShaderObjectARB(GLenum shaderType)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=1,
    error('invalid number of arguments');
end

r = moglcore( 'glCreateShaderObjectARB', shaderType );

return
