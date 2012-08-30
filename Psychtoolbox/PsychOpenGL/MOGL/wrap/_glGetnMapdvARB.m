function v = glGetnMapdvARB( target, query, bufSize )

% glGetnMapdvARB  Interface to OpenGL function glGetnMapdvARB
%
% usage:  v = glGetnMapdvARB( target, query, bufSize )
%
% C function:  void glGetnMapdvARB(GLenum target, GLenum query, GLsizei bufSize, GLdouble* v)

% 30-Aug-2012 -- created (generated automatically from header files)

% ---allocate---

if nargin~=3,
    error('invalid number of arguments');
end

v = double(0);

moglcore( 'glGetnMapdvARB', target, query, bufSize, v );

return
