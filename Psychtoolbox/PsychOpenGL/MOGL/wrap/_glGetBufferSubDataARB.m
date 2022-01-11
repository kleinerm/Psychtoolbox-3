function data = glGetBufferSubDataARB( target, ARB1, ARB2 )

% glGetBufferSubDataARB  Interface to OpenGL function glGetBufferSubDataARB
%
% usage:  data = glGetBufferSubDataARB( target, ARB1, ARB2 )
%
% C function:  void glGetBufferSubDataARB(GLenum target, GLintptr ARB, GLsizeiptr ARB, void* data)

% 30-Sep-2014 -- created (generated automatically from header files)

% ---allocate---

if nargin~=3,
    error('invalid number of arguments');
end

data = (0);

moglcore( 'glGetBufferSubDataARB', target, ARB1, ARB2, data );

return
