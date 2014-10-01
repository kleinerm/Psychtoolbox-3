function glInvalidateNamedFramebufferSubData( framebuffer, numAttachments, attachments, x, y, width, height )

% glInvalidateNamedFramebufferSubData  Interface to OpenGL function glInvalidateNamedFramebufferSubData
%
% usage:  glInvalidateNamedFramebufferSubData( framebuffer, numAttachments, attachments, x, y, width, height )
%
% C function:  void glInvalidateNamedFramebufferSubData(GLuint framebuffer, GLsizei numAttachments, const GLenum* attachments, GLint x, GLint y, GLsizei width, GLsizei height)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=7,
    error('invalid number of arguments');
end

moglcore( 'glInvalidateNamedFramebufferSubData', framebuffer, numAttachments, uint32(attachments), x, y, width, height );

return
