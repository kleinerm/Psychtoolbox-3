function values = glGetnMinmaxARB( target, reset, format, type, bufSize )

% glGetnMinmaxARB  Interface to OpenGL function glGetnMinmaxARB
%
% usage:  values = glGetnMinmaxARB( target, reset, format, type, bufSize )
%
% C function:  void glGetnMinmaxARB(GLenum target, GLboolean reset, GLenum format, GLenum type, GLsizei bufSize, GLvoid* values)

% 30-Aug-2012 -- created (generated automatically from header files)

% ---allocate---

if nargin~=5,
    error('invalid number of arguments');
end

values = (0);

moglcore( 'glGetnMinmaxARB', target, reset, format, type, bufSize, values );

return
