function glSpriteParameterfvSGIX( pname, params )

% glSpriteParameterfvSGIX  Interface to OpenGL function glSpriteParameterfvSGIX
%
% usage:  glSpriteParameterfvSGIX( pname, params )
%
% C function:  void glSpriteParameterfvSGIX(GLenum pname, const GLfloat* params)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=2,
    error('invalid number of arguments');
end

moglcore( 'glSpriteParameterfvSGIX', pname, single(params) );

return
