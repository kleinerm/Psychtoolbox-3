function r = glIsVariantEnabledEXT( id, cap )

% glIsVariantEnabledEXT  Interface to OpenGL function glIsVariantEnabledEXT
%
% usage:  r = glIsVariantEnabledEXT( id, cap )
%
% C function:  GLboolean glIsVariantEnabledEXT(GLuint id, GLenum cap)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=2,
    error('invalid number of arguments');
end

r = moglcore( 'glIsVariantEnabledEXT', id, cap );

return
