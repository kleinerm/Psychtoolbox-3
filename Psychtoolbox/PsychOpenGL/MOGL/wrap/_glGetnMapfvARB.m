function v = glGetnMapfvARB( target, query, bufSize )

% glGetnMapfvARB  Interface to OpenGL function glGetnMapfvARB
%
% usage:  v = glGetnMapfvARB( target, query, bufSize )
%
% C function:  void glGetnMapfvARB(GLenum target, GLenum query, GLsizei bufSize, GLfloat* v)

% 30-Aug-2012 -- created (generated automatically from header files)

% ---allocate---

if nargin~=3,
    error('invalid number of arguments');
end

v = single(0);

moglcore( 'glGetnMapfvARB', target, query, bufSize, v );

return
