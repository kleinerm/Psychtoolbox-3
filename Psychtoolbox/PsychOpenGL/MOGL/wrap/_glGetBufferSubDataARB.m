function data = glGetBufferSubDataARB( target, ARB, ARB )

% glGetBufferSubDataARB  Interface to OpenGL function glGetBufferSubDataARB
%
% usage:  data = glGetBufferSubDataARB( target, ARB, ARB )
%
% C function:  void glGetBufferSubDataARB(GLenum target, GLintptr ARB, GLsizeiptr ARB, void* data)

% 30-Sep-2014 -- created (generated automatically from header files)

% ---allocate---

if nargin~=3,
    error('invalid number of arguments');
end

data = (0);

moglcore( 'glGetBufferSubDataARB', target, ARB, ARB, data );

return
