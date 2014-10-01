function glBindBufferRange( target, index, buffer, offset, size )

% glBindBufferRange  Interface to OpenGL function glBindBufferRange
%
% usage:  glBindBufferRange( target, index, buffer, offset, size )
%
% C function:  void glBindBufferRange(GLenum target, GLuint index, GLuint buffer, GLintptr offset, GLsizeiptr size)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=5,
    error('invalid number of arguments');
end

moglcore( 'glBindBufferRange', target, index, buffer, offset, size );

return
