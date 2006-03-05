function v = glGetMapiv( target, query )

% glGetMapiv  Interface to OpenGL function glGetMapiv
%
% usage:  v = glGetMapiv( target, query )
%
% C function:  void glGetMapiv(GLenum target, GLenum query, GLint* v)

% 05-Mar-2006 -- created (generated automatically from header files)

% ---allocate---

if nargin~=2,
    error('invalid number of arguments');
end

v = int32(0);

moglcore( 'glGetMapiv', target, query, v );

return
