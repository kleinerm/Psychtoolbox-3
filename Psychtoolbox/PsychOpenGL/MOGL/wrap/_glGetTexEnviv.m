function params = glGetTexEnviv( target, pname )

% glGetTexEnviv  Interface to OpenGL function glGetTexEnviv
%
% usage:  params = glGetTexEnviv( target, pname )
%
% C function:  void glGetTexEnviv(GLenum target, GLenum pname, GLint* params)

% 05-Mar-2006 -- created (generated automatically from header files)

% ---allocate---

if nargin~=2,
    error('invalid number of arguments');
end

params = int32(0);

moglcore( 'glGetTexEnviv', target, pname, params );

return
