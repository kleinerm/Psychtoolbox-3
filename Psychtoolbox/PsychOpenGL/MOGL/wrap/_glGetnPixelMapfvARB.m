function values = glGetnPixelMapfvARB( map, bufSize )

% glGetnPixelMapfvARB  Interface to OpenGL function glGetnPixelMapfvARB
%
% usage:  values = glGetnPixelMapfvARB( map, bufSize )
%
% C function:  void glGetnPixelMapfvARB(GLenum map, GLsizei bufSize, GLfloat* values)

% 30-Aug-2012 -- created (generated automatically from header files)

% ---allocate---

if nargin~=2,
    error('invalid number of arguments');
end

values = single(0);

moglcore( 'glGetnPixelMapfvARB', map, bufSize, values );

return
