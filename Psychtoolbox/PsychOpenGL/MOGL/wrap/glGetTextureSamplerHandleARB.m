function r = glGetTextureSamplerHandleARB( texture, sampler )

% glGetTextureSamplerHandleARB  Interface to OpenGL function glGetTextureSamplerHandleARB
%
% usage:  r = glGetTextureSamplerHandleARB( texture, sampler )
%
% C function:  GLuint64 glGetTextureSamplerHandleARB(GLuint texture, GLuint sampler)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=2,
    error('invalid number of arguments');
end

r = moglcore( 'glGetTextureSamplerHandleARB', texture, sampler );

return
