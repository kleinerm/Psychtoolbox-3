function glMultiDrawElementsIndirectBindlessCountNV( mode, type, indirect, drawCount, maxDrawCount, stride, vertexBufferCount )

% glMultiDrawElementsIndirectBindlessCountNV  Interface to OpenGL function glMultiDrawElementsIndirectBindlessCountNV
%
% usage:  glMultiDrawElementsIndirectBindlessCountNV( mode, type, indirect, drawCount, maxDrawCount, stride, vertexBufferCount )
%
% C function:  void glMultiDrawElementsIndirectBindlessCountNV(GLenum mode, GLenum type, const void* indirect, GLsizei drawCount, GLsizei maxDrawCount, GLsizei stride, GLint vertexBufferCount)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=7,
    error('invalid number of arguments');
end

moglcore( 'glMultiDrawElementsIndirectBindlessCountNV', mode, type, indirect, drawCount, maxDrawCount, stride, vertexBufferCount );

return
