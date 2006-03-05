function params = glGetTexEnvfv( target, pname )

% glGetTexEnvfv  Interface to OpenGL function glGetTexEnvfv
%
% usage:  params = glGetTexEnvfv( target, pname )
%
% C function:  void glGetTexEnvfv(GLenum target, GLenum pname, GLfloat* params)

% 05-Mar-2006 -- created (generated automatically from header files)

% ---allocate---

if nargin~=2,
    error('invalid number of arguments');
end

params = single(0);

moglcore( 'glGetTexEnvfv', target, pname, params );

return
