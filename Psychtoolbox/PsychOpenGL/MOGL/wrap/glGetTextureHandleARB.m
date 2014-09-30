function r = glGetTextureHandleARB( texture )

% glGetTextureHandleARB  Interface to OpenGL function glGetTextureHandleARB
%
% usage:  r = glGetTextureHandleARB( texture )
%
% C function:  GLuint64 glGetTextureHandleARB(GLuint texture)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=1,
    error('invalid number of arguments');
end

r = moglcore( 'glGetTextureHandleARB', texture );

return
