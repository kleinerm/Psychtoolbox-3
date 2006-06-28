function glTexGenfv( coord, pname, params )

% glTexGenfv  Interface to OpenGL function glTexGenfv
%
% usage:  glTexGenfv( coord, pname, params )
%
% C function:  void glTexGenfv(GLenum coord, GLenum pname, const GLfloat* params)

% 05-Mar-2006 -- created (generated automatically from header files)

if nargin~=3,
    error('invalid number of arguments');
end

moglcore( 'glTexGenfv', coord, pname, moglsingle(params) );

return
