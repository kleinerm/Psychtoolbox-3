function glPixelZoom( xfactor, yfactor )

% glPixelZoom  Interface to OpenGL function glPixelZoom
%
% usage:  glPixelZoom( xfactor, yfactor )
%
% C function:  void glPixelZoom(GLfloat xfactor, GLfloat yfactor)

% 05-Mar-2006 -- created (generated automatically from header files)

if nargin~=2,
    error('invalid number of arguments');
end

moglcore( 'glPixelZoom', xfactor, yfactor );

return
