function values = glGetnPixelMapusvARB( map, bufSize )

% glGetnPixelMapusvARB  Interface to OpenGL function glGetnPixelMapusvARB
%
% usage:  values = glGetnPixelMapusvARB( map, bufSize )
%
% C function:  void glGetnPixelMapusvARB(GLenum map, GLsizei bufSize, GLushort* values)

% 30-Aug-2012 -- created (generated automatically from header files)

% ---allocate---

if nargin~=2,
    error('invalid number of arguments');
end

values = uint16(0);

moglcore( 'glGetnPixelMapusvARB', map, bufSize, values );

return
