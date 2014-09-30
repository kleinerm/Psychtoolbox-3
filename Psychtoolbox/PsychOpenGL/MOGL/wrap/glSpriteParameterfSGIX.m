function glSpriteParameterfSGIX( pname, param )

% glSpriteParameterfSGIX  Interface to OpenGL function glSpriteParameterfSGIX
%
% usage:  glSpriteParameterfSGIX( pname, param )
%
% C function:  void glSpriteParameterfSGIX(GLenum pname, GLfloat param)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=2,
    error('invalid number of arguments');
end

moglcore( 'glSpriteParameterfSGIX', pname, param );

return
