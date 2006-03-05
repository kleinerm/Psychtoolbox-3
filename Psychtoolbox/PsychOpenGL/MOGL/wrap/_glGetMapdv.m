function v = glGetMapdv( target, query )

% glGetMapdv  Interface to OpenGL function glGetMapdv
%
% usage:  v = glGetMapdv( target, query )
%
% C function:  void glGetMapdv(GLenum target, GLenum query, GLdouble* v)

% 05-Mar-2006 -- created (generated automatically from header files)

% ---allocate---

if nargin~=2,
    error('invalid number of arguments');
end

v = double(0);

moglcore( 'glGetMapdv', target, query, v );

return
