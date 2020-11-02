function glMulticastBlitFramebufferNV( srcGpu, dstGpu, srcX0, srcY0, srcX1, srcY1, dstX0, dstY0, dstX1, dstY1, mask, filter )

% glMulticastBlitFramebufferNV  Interface to OpenGL function glMulticastBlitFramebufferNV
%
% usage:  glMulticastBlitFramebufferNV( srcGpu, dstGpu, srcX0, srcY0, srcX1, srcY1, dstX0, dstY0, dstX1, dstY1, mask, filter )
%
% C function:  void glMulticastBlitFramebufferNV(GLuint srcGpu, GLuint dstGpu, GLint srcX0, GLint srcY0, GLint srcX1, GLint srcY1, GLint dstX0, GLint dstY0, GLint dstX1, GLint dstY1, GLbitfield mask, GLenum filter)

% 08-Aug-2020 -- created (generated automatically from header files)

if nargin~=12,
    error('invalid number of arguments');
end

moglcore( 'glMulticastBlitFramebufferNV', srcGpu, dstGpu, srcX0, srcY0, srcX1, srcY1, dstX0, dstY0, dstX1, dstY1, mask, filter );

return
