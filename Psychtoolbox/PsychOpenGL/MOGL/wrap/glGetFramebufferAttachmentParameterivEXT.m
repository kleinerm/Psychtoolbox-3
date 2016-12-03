function params = glGetFramebufferAttachmentParameterivEXT( target, attachment, pname )

% glGetFramebufferAttachmentParameterivEXT  Interface to OpenGL function glGetFramebufferAttachmentParameterivEXT
%
% usage:  params = glGetFramebufferAttachmentParameterivEXT( target, attachment, pname )
%
% C function:  void glGetFramebufferAttachmentParameterivEXT(GLenum target, GLenum attachment, GLenum pname, GLint* params)

% 30-May-2006 -- created (generated automatically from header files)

% ---allocate---
% ---protected---

if nargin~=3,
    error('invalid number of arguments');
end

params = int32([1,0]);

moglcore( 'glGetFramebufferAttachmentParameterivEXT', target, attachment, pname, params );
params = params(1:end-1);
return
% ---skip---
