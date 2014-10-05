function glBindBuffersRange( target, first, count, buffers, offsets, sizes )

% glBindBuffersRange  Interface to OpenGL function glBindBuffersRange
%
% usage:  glBindBuffersRange( target, first, count, buffers, offsets, sizes )
%
% C function:  void glBindBuffersRange(GLenum target, GLuint first, GLsizei count, const GLuint* buffers, const GLintptr* offsets, const GLsizeiptr* sizes)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=6,
    error('invalid number of arguments');
end

moglcore( 'glBindBuffersRange', target, first, count, uint32(buffers), int64(offsets), int64(sizes) );

return
