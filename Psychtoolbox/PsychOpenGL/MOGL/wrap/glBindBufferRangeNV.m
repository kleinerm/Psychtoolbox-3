function glBindBufferRangeNV( target, index, buffer, offset, size )

% glBindBufferRangeNV  Interface to OpenGL function glBindBufferRangeNV
%
% usage:  glBindBufferRangeNV( target, index, buffer, offset, size )
%
% C function:  void glBindBufferRangeNV(GLenum target, GLuint index, GLuint buffer, GLintptr offset, GLsizeiptr size)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=5,
    error('invalid number of arguments');
end

moglcore( 'glBindBufferRangeNV', target, index, buffer, offset, size );

return
