function glPixelStoref( pname, param )

% glPixelStoref  Interface to OpenGL function glPixelStoref
%
% usage:  glPixelStoref( pname, param )
%
% C function:  void glPixelStoref(GLenum pname, GLfloat param)

% 05-Mar-2006 -- created (generated automatically from header files)

if nargin~=2,
    error('invalid number of arguments');
end

moglcore( 'glPixelStoref', pname, param );

return
