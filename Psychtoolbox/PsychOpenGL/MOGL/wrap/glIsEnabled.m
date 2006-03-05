function r = glIsEnabled( cap )

% glIsEnabled  Interface to OpenGL function glIsEnabled
%
% usage:  r = glIsEnabled( cap )
%
% C function:  GLboolean glIsEnabled(GLenum cap)

% 05-Mar-2006 -- created (generated automatically from header files)

if nargin~=1,
    error('invalid number of arguments');
end

r = moglcore( 'glIsEnabled', cap );

return
