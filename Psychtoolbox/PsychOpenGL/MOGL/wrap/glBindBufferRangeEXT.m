function glBindBufferRangeEXT( target, index, buffer, offset, size )

% glBindBufferRangeEXT  Interface to OpenGL function glBindBufferRangeEXT
%
% usage:  glBindBufferRangeEXT( target, index, buffer, offset, size )
%
% C function:  void glBindBufferRangeEXT(GLenum target, GLuint index, GLuint buffer, GLintptr offset, GLsizeiptr size)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=5,
    error('invalid number of arguments');
end

moglcore( 'glBindBufferRangeEXT', target, index, buffer, offset, size );

return
