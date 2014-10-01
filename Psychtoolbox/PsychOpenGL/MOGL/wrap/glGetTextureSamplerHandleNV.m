function r = glGetTextureSamplerHandleNV( texture, sampler )

% glGetTextureSamplerHandleNV  Interface to OpenGL function glGetTextureSamplerHandleNV
%
% usage:  r = glGetTextureSamplerHandleNV( texture, sampler )
%
% C function:  GLuint64 glGetTextureSamplerHandleNV(GLuint texture, GLuint sampler)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=2,
    error('invalid number of arguments');
end

r = moglcore( 'glGetTextureSamplerHandleNV', texture, sampler );

return
