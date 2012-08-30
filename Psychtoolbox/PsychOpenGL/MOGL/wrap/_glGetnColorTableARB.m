function table = glGetnColorTableARB( target, format, type, bufSize )

% glGetnColorTableARB  Interface to OpenGL function glGetnColorTableARB
%
% usage:  table = glGetnColorTableARB( target, format, type, bufSize )
%
% C function:  void glGetnColorTableARB(GLenum target, GLenum format, GLenum type, GLsizei bufSize, GLvoid* table)

% 30-Aug-2012 -- created (generated automatically from header files)

% ---allocate---

if nargin~=4,
    error('invalid number of arguments');
end

table = (0);

moglcore( 'glGetnColorTableARB', target, format, type, bufSize, table );

return
