function glVertexArrayVertexBuffers( vaobj, first, count, buffers, offsets, strides )

% glVertexArrayVertexBuffers  Interface to OpenGL function glVertexArrayVertexBuffers
%
% usage:  glVertexArrayVertexBuffers( vaobj, first, count, buffers, offsets, strides )
%
% C function:  void glVertexArrayVertexBuffers(GLuint vaobj, GLuint first, GLsizei count, const GLuint* buffers, const GLintptr* offsets, const GLsizei* strides)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=6,
    error('invalid number of arguments');
end

moglcore( 'glVertexArrayVertexBuffers', vaobj, first, count, uint32(buffers), int64(offsets), int32(strides) );

return
