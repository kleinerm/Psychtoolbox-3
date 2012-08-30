function v = glGetnMapivARB( target, query, bufSize )

% glGetnMapivARB  Interface to OpenGL function glGetnMapivARB
%
% usage:  v = glGetnMapivARB( target, query, bufSize )
%
% C function:  void glGetnMapivARB(GLenum target, GLenum query, GLsizei bufSize, GLint* v)

% 30-Aug-2012 -- created (generated automatically from header files)

% ---allocate---

if nargin~=3,
    error('invalid number of arguments');
end

v = int32(0);

moglcore( 'glGetnMapivARB', target, query, bufSize, v );

return
