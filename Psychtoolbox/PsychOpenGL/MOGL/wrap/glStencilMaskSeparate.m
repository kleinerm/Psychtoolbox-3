function glStencilMaskSeparate( face, mask )

% glStencilMaskSeparate  Interface to OpenGL function glStencilMaskSeparate
%
% usage:  glStencilMaskSeparate( face, mask )
%
% C function:  void glStencilMaskSeparate(GLenum face, GLuint mask)

% 25-Mar-2011 -- created (generated automatically from header files)

if nargin~=2,
    error('invalid number of arguments');
end

moglcore( 'glStencilMaskSeparate', face, mask );

return
