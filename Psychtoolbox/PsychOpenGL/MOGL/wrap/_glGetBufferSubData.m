function data = glGetBufferSubData( target, ptr, ptr )

% glGetBufferSubData  Interface to OpenGL function glGetBufferSubData
%
% usage:  data = glGetBufferSubData( target, ptr, ptr )
%
% C function:  void glGetBufferSubData(GLenum target, GLint ptr, GLsizei ptr, GLvoid* data)

% 05-Mar-2006 -- created (generated automatically from header files)

% ---allocate---

if nargin~=3,
    error('invalid number of arguments');
end

data = (0);

moglcore( 'glGetBufferSubData', target, ptr, ptr, data );

return
