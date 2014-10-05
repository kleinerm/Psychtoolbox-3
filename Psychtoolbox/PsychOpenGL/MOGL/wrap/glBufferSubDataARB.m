function glBufferSubDataARB( target, ARB, ARB, data )

% glBufferSubDataARB  Interface to OpenGL function glBufferSubDataARB
%
% usage:  glBufferSubDataARB( target, ARB, ARB, data )
%
% C function:  void glBufferSubDataARB(GLenum target, GLintptr ARB, GLsizeiptr ARB, const void* data)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=4,
    error('invalid number of arguments');
end

moglcore( 'glBufferSubDataARB', target, ARB, ARB, data );

return
