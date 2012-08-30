function image = glGetnConvolutionFilterARB( target, format, type, bufSize )

% glGetnConvolutionFilterARB  Interface to OpenGL function glGetnConvolutionFilterARB
%
% usage:  image = glGetnConvolutionFilterARB( target, format, type, bufSize )
%
% C function:  void glGetnConvolutionFilterARB(GLenum target, GLenum format, GLenum type, GLsizei bufSize, GLvoid* image)

% 30-Aug-2012 -- created (generated automatically from header files)

% ---allocate---

if nargin~=4,
    error('invalid number of arguments');
end

image = (0);

moglcore( 'glGetnConvolutionFilterARB', target, format, type, bufSize, image );

return
