function glPixelStoref( pname, param )

% glPixelStoref  Interface to OpenGL function glPixelStoref
%
% usage:  glPixelStoref( pname, param )
%
% C function:  void glPixelStoref(GLenum pname, GLfloat param)

% 25-Mar-2011 -- created (generated automatically from header files)

if nargin~=2,
    error('invalid number of arguments');
end

moglcore( 'glPixelStoref', pname, param );

return
