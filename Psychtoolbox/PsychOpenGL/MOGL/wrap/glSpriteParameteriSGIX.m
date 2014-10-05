function glSpriteParameteriSGIX( pname, param )

% glSpriteParameteriSGIX  Interface to OpenGL function glSpriteParameteriSGIX
%
% usage:  glSpriteParameteriSGIX( pname, param )
%
% C function:  void glSpriteParameteriSGIX(GLenum pname, GLint param)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=2,
    error('invalid number of arguments');
end

moglcore( 'glSpriteParameteriSGIX', pname, param );

return
