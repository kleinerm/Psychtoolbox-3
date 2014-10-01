function glMultiDrawArraysIndirectBindlessNV( mode, indirect, drawCount, stride, vertexBufferCount )

% glMultiDrawArraysIndirectBindlessNV  Interface to OpenGL function glMultiDrawArraysIndirectBindlessNV
%
% usage:  glMultiDrawArraysIndirectBindlessNV( mode, indirect, drawCount, stride, vertexBufferCount )
%
% C function:  void glMultiDrawArraysIndirectBindlessNV(GLenum mode, const void* indirect, GLsizei drawCount, GLsizei stride, GLint vertexBufferCount)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=5,
    error('invalid number of arguments');
end

moglcore( 'glMultiDrawArraysIndirectBindlessNV', mode, indirect, drawCount, stride, vertexBufferCount );

return
