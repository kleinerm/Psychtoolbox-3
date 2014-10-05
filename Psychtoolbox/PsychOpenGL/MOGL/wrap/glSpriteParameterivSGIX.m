function glSpriteParameterivSGIX( pname, params )

% glSpriteParameterivSGIX  Interface to OpenGL function glSpriteParameterivSGIX
%
% usage:  glSpriteParameterivSGIX( pname, params )
%
% C function:  void glSpriteParameterivSGIX(GLenum pname, const GLint* params)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=2,
    error('invalid number of arguments');
end

moglcore( 'glSpriteParameterivSGIX', pname, int32(params) );

return
