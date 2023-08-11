function texid = moglGetTexForFBO(fbo, bufferid)
% texid = moglGetTexForFBO(fbo, bufferid) -- Return texture name of texture which is
% bound to the specified 'fbo' Framebuffer object as color buffer number 'bufferid'.
% bufferid defaults to 1, aka the first attached color buffer.

% History:
% 30.5.2006 Written (MK).
global GL;

% Child protection:
AssertGLSL;

if nargin < 1
    error('moglGetTexForFBO called without fbo argument.');
end;

if fbo == 0
    texid = 0;
    return;
end;

if nargin < 2
    bufferid=1;
end;

if isempty(bufferid)
    bufferid=1;
end;

% Child protection:
if ~glIsFramebufferEXT(fbo)
    error('Invalid fbo identifier passed. This is not a valid FBO.');
end;

boundfbo = glGetIntegerv(GL.FRAMEBUFFER_BINDING_EXT);
if boundfbo~=fbo
    % fbo currently unbound. Bind it:
    glBindFramebufferEXT(GL.FRAMEBUFFER_EXT, fbo);    
end;

% Query its texture attachments and destroy them:
texid = glGetFramebufferAttachmentParameterivEXT(GL.FRAMEBUFFER_EXT, GL.COLOR_ATTACHMENT0_EXT + bufferid - 1, GL.FRAMEBUFFER_ATTACHMENT_OBJECT_NAME_EXT);

% Rebind previous fbo, unless we were bound:
if boundfbo~=fbo
    % Reset to last bound FBO:
    glBindFramebufferEXT(GL.FRAMEBUFFER_EXT, boundfbo);
end;

% Done.
return;
