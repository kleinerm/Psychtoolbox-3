function values = glGetnPixelMapuivARB( map, bufSize )

% glGetnPixelMapuivARB  Interface to OpenGL function glGetnPixelMapuivARB
%
% usage:  values = glGetnPixelMapuivARB( map, bufSize )
%
% C function:  void glGetnPixelMapuivARB(GLenum map, GLsizei bufSize, GLuint* values)

% 30-Aug-2012 -- created (generated automatically from header files)

% ---allocate---

if nargin~=2,
    error('invalid number of arguments');
end

values = uint32(0);

moglcore( 'glGetnPixelMapuivARB', map, bufSize, values );

return
