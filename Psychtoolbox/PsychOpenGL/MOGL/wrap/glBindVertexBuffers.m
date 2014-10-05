function glBindVertexBuffers( first, count, buffers, offsets, strides )

% glBindVertexBuffers  Interface to OpenGL function glBindVertexBuffers
%
% usage:  glBindVertexBuffers( first, count, buffers, offsets, strides )
%
% C function:  void glBindVertexBuffers(GLuint first, GLsizei count, const GLuint* buffers, const GLintptr* offsets, const GLsizei* strides)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=5,
    error('invalid number of arguments');
end

moglcore( 'glBindVertexBuffers', first, count, uint32(buffers), int64(offsets), int32(strides) );

return
