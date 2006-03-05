function glStencilMask( mask )

% glStencilMask  Interface to OpenGL function glStencilMask
%
% usage:  glStencilMask( mask )
%
% C function:  void glStencilMask(GLuint mask)

% 05-Mar-2006 -- created (generated automatically from header files)

if nargin~=1,
    error('invalid number of arguments');
end

moglcore( 'glStencilMask', mask );

return
