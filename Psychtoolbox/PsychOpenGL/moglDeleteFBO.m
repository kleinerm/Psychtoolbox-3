function moglDeleteFBO(fbo)
% moglDeleteFBO(fbo) -- Delete FBO 'fbo' and its associated buffers and textures.
%
% This function is mostly useful for image processing and GPGPU
% applications. Normal Psychtoolbox code will want to use standard
% Offscreen windows instead. They are properly managed by Psychtoolbox for
% normal stimulus drawing and implemented to work on any kind of hardware,
% whereas this function is optimized for OpenGL-2 compliant hardware.

% History:
% 30.05.2006 Wrote it (MK).

global GL;

% Child protection:
AssertGLSL;

if nargin < 1
    error('moglDeleteFBO called without fbo argument.');
end;

if fbo == 0
    return;
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
for i=1:glGetIntegerv(GL.MAX_COLOR_ATTACHMENTS_EXT)
    if glGetFramebufferAttachmentParameterivEXT(GL.FRAMEBUFFER_EXT, GL.COLOR_ATTACHMENT0_EXT + i - 1, GL.FRAMEBUFFER_ATTACHMENT_OBJECT_TYPE_EXT)==GL.TEXTURE
        texid = glGetFramebufferAttachmentParameterivEXT(GL.FRAMEBUFFER_EXT, GL.COLOR_ATTACHMENT0_EXT, GL.FRAMEBUFFER_ATTACHMENT_OBJECT_NAME_EXT);
        glDeleteTextures(1, texid);
    end;
end;

% Rebind previous fbo, unless we were bound already:
if boundfbo==fbo
    % Reset to system framebuffer:
    glBindFramebufferEXT(GL.FRAMEBUFFER_EXT, 0);    
else
    % Reset to last bound FBO:
    glBindFramebufferEXT(GL.FRAMEBUFFER_EXT, boundfbo);
end;

% Finally destroy the FBo itself:
glDeleteFramebuffersEXT( 1, fbo);

% Done.
return;
