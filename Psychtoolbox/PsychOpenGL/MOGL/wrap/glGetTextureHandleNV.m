function r = glGetTextureHandleNV( texture )

% glGetTextureHandleNV  Interface to OpenGL function glGetTextureHandleNV
%
% usage:  r = glGetTextureHandleNV( texture )
%
% C function:  GLuint64 glGetTextureHandleNV(GLuint texture)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=1,
    error('invalid number of arguments');
end

r = moglcore( 'glGetTextureHandleNV', texture );

return
