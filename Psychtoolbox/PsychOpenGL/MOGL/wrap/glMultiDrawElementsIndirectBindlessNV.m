function glMultiDrawElementsIndirectBindlessNV( mode, type, indirect, drawCount, stride, vertexBufferCount )

% glMultiDrawElementsIndirectBindlessNV  Interface to OpenGL function glMultiDrawElementsIndirectBindlessNV
%
% usage:  glMultiDrawElementsIndirectBindlessNV( mode, type, indirect, drawCount, stride, vertexBufferCount )
%
% C function:  void glMultiDrawElementsIndirectBindlessNV(GLenum mode, GLenum type, const void* indirect, GLsizei drawCount, GLsizei stride, GLint vertexBufferCount)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=6,
    error('invalid number of arguments');
end

moglcore( 'glMultiDrawElementsIndirectBindlessNV', mode, type, indirect, drawCount, stride, vertexBufferCount );

return
