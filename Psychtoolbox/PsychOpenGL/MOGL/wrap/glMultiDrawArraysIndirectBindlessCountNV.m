function glMultiDrawArraysIndirectBindlessCountNV( mode, indirect, drawCount, maxDrawCount, stride, vertexBufferCount )

% glMultiDrawArraysIndirectBindlessCountNV  Interface to OpenGL function glMultiDrawArraysIndirectBindlessCountNV
%
% usage:  glMultiDrawArraysIndirectBindlessCountNV( mode, indirect, drawCount, maxDrawCount, stride, vertexBufferCount )
%
% C function:  void glMultiDrawArraysIndirectBindlessCountNV(GLenum mode, const void* indirect, GLsizei drawCount, GLsizei maxDrawCount, GLsizei stride, GLint vertexBufferCount)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=6,
    error('invalid number of arguments');
end

moglcore( 'glMultiDrawArraysIndirectBindlessCountNV', mode, indirect, drawCount, maxDrawCount, stride, vertexBufferCount );

return
