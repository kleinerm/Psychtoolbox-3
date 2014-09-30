function glBufferStorage( target, size, data, flags )

% glBufferStorage  Interface to OpenGL function glBufferStorage
%
% usage:  glBufferStorage( target, size, data, flags )
%
% C function:  void glBufferStorage(GLenum target, GLsizeiptr size, const void* data, GLbitfield flags)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=4,
    error('invalid number of arguments');
end

moglcore( 'glBufferStorage', target, size, data, flags );

return
