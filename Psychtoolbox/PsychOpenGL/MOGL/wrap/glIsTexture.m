function r = glIsTexture( texture )

% glIsTexture  Interface to OpenGL function glIsTexture
%
% usage:  r = glIsTexture( texture )
%
% C function:  GLboolean glIsTexture(GLuint texture)

% 05-Mar-2006 -- created (generated automatically from header files)

if nargin~=1,
    error('invalid number of arguments');
end

r = moglcore( 'glIsTexture', texture );

return
