function params = glGetTexGeniv( coord, pname )

% glGetTexGeniv  Interface to OpenGL function glGetTexGeniv
%
% usage:  params = glGetTexGeniv( coord, pname )
%
% C function:  void glGetTexGeniv(GLenum coord, GLenum pname, GLint* params)

% 05-Mar-2006 -- created (generated automatically from header files)

% ---allocate---

if nargin~=2,
    error('invalid number of arguments');
end

params = int32(0);

moglcore( 'glGetTexGeniv', coord, pname, params );

return
