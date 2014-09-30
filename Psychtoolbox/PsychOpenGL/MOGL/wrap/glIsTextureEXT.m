function r = glIsTextureEXT( texture )

% glIsTextureEXT  Interface to OpenGL function glIsTextureEXT
%
% usage:  r = glIsTextureEXT( texture )
%
% C function:  GLboolean glIsTextureEXT(GLuint texture)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=1,
    error('invalid number of arguments');
end

r = moglcore( 'glIsTextureEXT', texture );

return
