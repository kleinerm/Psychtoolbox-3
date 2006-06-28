function v = glGetMapfv( target, query )

% glGetMapfv  Interface to OpenGL function glGetMapfv
%
% usage:  v = glGetMapfv( target, query )
%
% C function:  void glGetMapfv(GLenum target, GLenum query, GLfloat* v)

% 05-Mar-2006 -- created (generated automatically from header files)

% ---allocate---

if nargin~=2,
    error('invalid number of arguments');
end

v = moglsingle(0);

moglcore( 'glGetMapfv', target, query, v );

return
