function glInvalidateNamedFramebufferData( framebuffer, numAttachments, attachments )

% glInvalidateNamedFramebufferData  Interface to OpenGL function glInvalidateNamedFramebufferData
%
% usage:  glInvalidateNamedFramebufferData( framebuffer, numAttachments, attachments )
%
% C function:  void glInvalidateNamedFramebufferData(GLuint framebuffer, GLsizei numAttachments, const GLenum* attachments)

% 30-Sep-2014 -- created (generated automatically from header files)

if nargin~=3,
    error('invalid number of arguments');
end

moglcore( 'glInvalidateNamedFramebufferData', framebuffer, numAttachments, uint32(attachments) );

return
