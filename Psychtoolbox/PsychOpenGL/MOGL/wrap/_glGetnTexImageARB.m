function img = glGetnTexImageARB( target, level, format, type, bufSize )

% glGetnTexImageARB  Interface to OpenGL function glGetnTexImageARB
%
% usage:  img = glGetnTexImageARB( target, level, format, type, bufSize )
%
% C function:  void glGetnTexImageARB(GLenum target, GLint level, GLenum format, GLenum type, GLsizei bufSize, GLvoid* img)

% 30-Aug-2012 -- created (generated automatically from header files)

% ---allocate---

if nargin~=5,
    error('invalid number of arguments');
end

img = (0);

moglcore( 'glGetnTexImageARB', target, level, format, type, bufSize, img );

return
