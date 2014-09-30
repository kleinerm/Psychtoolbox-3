function glBufferDataARB( target, ARB, data, usage )

% glBufferDataARB  Interface to OpenGL function glBufferDataARB
%
% usage:  glBufferDataARB( target, ARB, data, usage )
%
% C function:  void glBufferDataARB(GLenum target, GLsizeiptr ARB, const void* data, GLenum usage)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=4,
    error('invalid number of arguments');
end

moglcore( 'glBufferDataARB', target, ARB, data, usage );

return
