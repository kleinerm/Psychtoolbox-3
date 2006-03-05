function params = glGetTexGenfv( coord, pname )

% glGetTexGenfv  Interface to OpenGL function glGetTexGenfv
%
% usage:  params = glGetTexGenfv( coord, pname )
%
% C function:  void glGetTexGenfv(GLenum coord, GLenum pname, GLfloat* params)

% 05-Mar-2006 -- created (generated automatically from header files)

% ---allocate---

if nargin~=2,
    error('invalid number of arguments');
end

params = single(0);

moglcore( 'glGetTexGenfv', coord, pname, params );

return
