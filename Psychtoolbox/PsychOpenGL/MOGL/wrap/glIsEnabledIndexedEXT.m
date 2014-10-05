function r = glIsEnabledIndexedEXT( target, index )

% glIsEnabledIndexedEXT  Interface to OpenGL function glIsEnabledIndexedEXT
%
% usage:  r = glIsEnabledIndexedEXT( target, index )
%
% C function:  GLboolean glIsEnabledIndexedEXT(GLenum target, GLuint index)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=2,
    error('invalid number of arguments');
end

r = moglcore( 'glIsEnabledIndexedEXT', target, index );

return
