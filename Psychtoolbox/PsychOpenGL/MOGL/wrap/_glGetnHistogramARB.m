function values = glGetnHistogramARB( target, reset, format, type, bufSize )

% glGetnHistogramARB  Interface to OpenGL function glGetnHistogramARB
%
% usage:  values = glGetnHistogramARB( target, reset, format, type, bufSize )
%
% C function:  void glGetnHistogramARB(GLenum target, GLboolean reset, GLenum format, GLenum type, GLsizei bufSize, GLvoid* values)

% 30-Aug-2012 -- created (generated automatically from header files)

% ---allocate---

if nargin~=5,
    error('invalid number of arguments');
end

values = (0);

moglcore( 'glGetnHistogramARB', target, reset, format, type, bufSize, values );

return
